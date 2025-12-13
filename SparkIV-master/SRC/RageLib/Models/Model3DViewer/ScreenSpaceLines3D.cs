//---------------------------------------------------------------------------
//
// (c) Copyright Microsoft Corporation.
// This source is subject to the Microsoft Limited Permissive License.
// See http://www.microsoft.com/resources/sharedsource/licensingbasics/limitedpermissivelicense.mspx
// All other rights reserved.
//
// This file is part of the 3D Tools for Windows Presentation Foundation
// project.  For more information, see:
// 
// http://CodePlex.com/Wiki/View.aspx?ProjectName=3DTools
//
//---------------------------------------------------------------------------

using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Media3D;

namespace RageLib.Models.Model3DViewer
{
    /// <summary>
    ///     ScreenSpaceLines3D are a 3D line primitive whose thickness
    ///     is constant in 2D space post projection.
    /// 
    ///     This means that the lines do not become foreshortened as
    ///     they receed from the camera as other 3D primitives do under
    ///     a typical perspective projection.
    /// 
    ///     Example Usage:
    /// 
    ///     &lt;tools:ScreenSpaceLines3D
    ///         Points="0,0,0 0,1,0 0,1,0 1,1,0 1,1,0 0,0,1"
    ///         Thickness="5" Color="Red"&gt;
    /// 
    ///     "Screen space" is a bit of a misnomer as the line thickness
    ///     is specified in the 2D coordinate system of the container
    ///     Viewport3D, not the screen.
    /// </summary>
    public class ScreenSpaceLines3D : ModelVisual3D
    {
        public ScreenSpaceLines3D()
        {
            _mesh = new MeshGeometry3D();
            _model = new GeometryModel3D();
            _model.Geometry = _mesh;
            SetColor(this.Color);

            this.Content = _model;
            this.Points = new Point3DCollection();

            CompositionTarget.Rendering += OnRender;
        }

        public static readonly DependencyProperty ColorProperty =
            DependencyProperty.Register(
                "Color",
                typeof(Color),
                typeof(ScreenSpaceLines3D),
                new PropertyMetadata(
                    Colors.White,
                    OnColorChanged));

        private static void OnColorChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
        {
            ((ScreenSpaceLines3D)sender).SetColor((Color)args.NewValue);
        }

        private void SetColor(Color color)
        {
            MaterialGroup unlitMaterial = new MaterialGroup();
            unlitMaterial.Children.Add(new DiffuseMaterial(new SolidColorBrush(Colors.Black)));
            unlitMaterial.Children.Add(new EmissiveMaterial(new SolidColorBrush(color)));
            unlitMaterial.Freeze();

            _model.Material = unlitMaterial;
            _model.BackMaterial = unlitMaterial;
        }

        public Color Color
        {
            get { return (Color)GetValue(ColorProperty); }
            set { SetValue(ColorProperty, value); }
        }

        public static readonly DependencyProperty ThicknessProperty =
            DependencyProperty.Register(
                "Thickness",
                typeof(double),
                typeof(ScreenSpaceLines3D),
                new PropertyMetadata(
                    1.0,
                    OnThicknessChanged));

        private static void OnThicknessChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
        {
            ((ScreenSpaceLines3D)sender).GeometryDirty();
        }

        public double Thickness
        {
            get { return (double)GetValue(ThicknessProperty); }
            set { SetValue(ThicknessProperty, value); }
        }

        public static readonly DependencyProperty PointsProperty =
            DependencyProperty.Register(
                "Points",
                typeof(Point3DCollection),
                typeof(ScreenSpaceLines3D),
                new PropertyMetadata(
                    null,
                    OnPointsChanged));

        private static void OnPointsChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
        {
            ((ScreenSpaceLines3D)sender).GeometryDirty();
        }

        public Point3DCollection Points
        {
            get { return (Point3DCollection)GetValue(PointsProperty); }
            set { SetValue(PointsProperty, value); }
        }

        private void OnRender(object sender, EventArgs e)
        {
            if (Points.Count == 0 && _mesh.Positions.Count == 0)
            {
                return;
            }

            if (UpdateTransforms())
            {
                RebuildGeometry();
            }
        }

        private void GeometryDirty()
        {
            // Force next call to UpdateTransforms() to return true.
            _visualToScreen = MathUtils.ZeroMatrix;
        }

        private void RebuildGeometry()
        {
            double halfThickness = Thickness / 2.0;
            int numLines = Points.Count / 2;

            Point3DCollection positions = new Point3DCollection(numLines * 4);

            for (int i = 0; i < numLines; i++)
            {
                int startIndex = i * 2;

                Point3D startPoint = Points[startIndex];
                Point3D endPoint = Points[startIndex + 1];

                AddSegment(positions, startPoint, endPoint, halfThickness);
            }

            positions.Freeze();
            _mesh.Positions = positions;

            Int32Collection indices = new Int32Collection(Points.Count * 3);

            for (int i = 0; i < Points.Count / 2; i++)
            {
                indices.Add(i * 4 + 2);
                indices.Add(i * 4 + 1);
                indices.Add(i * 4 + 0);

                indices.Add(i * 4 + 2);
                indices.Add(i * 4 + 3);
                indices.Add(i * 4 + 1);
            }

            indices.Freeze();
            _mesh.TriangleIndices = indices;
        }

        private void AddSegment(Point3DCollection positions, Point3D startPoint, Point3D endPoint, double halfThickness)
        {
            // NOTE: We want the vector below to be perpendicular post projection so
            //       we need to compute the line direction in post-projective space.
            Vector3D lineDirection = endPoint * _visualToScreen - startPoint * _visualToScreen;
            lineDirection.Z = 0;
            lineDirection.Normalize();

            // NOTE: Implicit Rot(90) during construction to get a perpendicular vector.
            Vector delta = new Vector(-lineDirection.Y, lineDirection.X);
            delta *= halfThickness;

            Point3D pOut1, pOut2;

            Widen(startPoint, delta, out pOut1, out pOut2);

            positions.Add(pOut1);
            positions.Add(pOut2);

            Widen(endPoint, delta, out pOut1, out pOut2);

            positions.Add(pOut1);
            positions.Add(pOut2);
        }

        private void Widen(Point3D pIn, Vector delta, out Point3D pOut1, out Point3D pOut2)
        {
            Point4D pIn4 = (Point4D)pIn;
            Point4D pOut41 = pIn4 * _visualToScreen;
            Point4D pOut42 = pOut41;

            pOut41.X += delta.X * pOut41.W;
            pOut41.Y += delta.Y * pOut41.W;

            pOut42.X -= delta.X * pOut42.W;
            pOut42.Y -= delta.Y * pOut42.W;

            pOut41 *= _screenToVisual;
            pOut42 *= _screenToVisual;

            // NOTE: Z is not modified above, so we use the original Z below.

            pOut1 = new Point3D(
                pOut41.X / pOut41.W,
                pOut41.Y / pOut41.W,
                pOut41.Z / pOut41.W);

            pOut2 = new Point3D(
                pOut42.X / pOut42.W,
                pOut42.Y / pOut42.W,
                pOut42.Z / pOut42.W);
        }

        private bool UpdateTransforms()
        {
            Viewport3DVisual viewport;
            bool success;

            Matrix3D visualToScreen = MathUtils.TryTransformTo2DAncestor(this, out viewport, out success);

            if (!success || !visualToScreen.HasInverse)
            {
                _mesh.Positions = null;
                return false;
            }

            if (visualToScreen == _visualToScreen)
            {
                return false;
            }

            _visualToScreen = _screenToVisual = visualToScreen;
            _screenToVisual.Invert();

            return true;
        }

        #region MakeWireframe

        public void MakeWireframe(Model3D model)
        {
            this.Points.Clear();

            if (model == null)
            {
                return;
            }

            Matrix3DStack transform = new Matrix3DStack();
            transform.Push(Matrix3D.Identity);

            WireframeHelper(model, transform);
        }

        private void WireframeHelper(Model3D model, Matrix3DStack matrixStack)
        {
            Transform3D transform = model.Transform;

            if (transform != null && transform != Transform3D.Identity)
            {
                matrixStack.Prepend(model.Transform.Value);
            }

            try
            {
                Model3DGroup group = model as Model3DGroup;

                if (group != null)
                {
                    WireframeHelper(group, matrixStack);
                    return;
                }

                GeometryModel3D geometry = model as GeometryModel3D;

                if (geometry != null)
                {
                    WireframeHelper(geometry, matrixStack);
                    return;
                }
            }
            finally
            {
                if (transform != null && transform != Transform3D.Identity)
                {
                    matrixStack.Pop();
                }
            }
        }

        private void WireframeHelper(Model3DGroup group, Matrix3DStack matrixStack)
        {
            foreach (Model3D child in group.Children)
            {
                WireframeHelper(child, matrixStack);
            }
        }

        private void WireframeHelper(GeometryModel3D model, Matrix3DStack matrixStack)
        {
            Geometry3D geometry = model.Geometry;
            MeshGeometry3D mesh = geometry as MeshGeometry3D;

            if (mesh != null)
            {
                Point3D[] positions = new Point3D[mesh.Positions.Count];
                mesh.Positions.CopyTo(positions, 0);
                matrixStack.Peek().Transform(positions);

                Int32Collection indices = mesh.TriangleIndices;

                if (indices.Count > 0)
                {
                    int limit = positions.Length - 1;

                    for (int i = 2, count = indices.Count; i < count; i += 3)
                    {
                        int i0 = indices[i - 2];
                        int i1 = indices[i - 1];
                        int i2 = indices[i];

                        // WPF halts rendering on the first deformed triangle.  We should
                        // do the same.
                        if ((0 > i0 || i0 > limit)
                            || (0 > i1 || i1 > limit)
                            || (0 > i2 || i2 > limit))
                        {
                            break;
                        }

                        AddTriangle(positions, i0, i1, i2);
                    }
                }
                else
                {
                    for (int i = 2, count = positions.Length; i < count; i += 3)
                    {
                        int i0 = i - 2;
                        int i1 = i - 1;
                        int i2 = i;

                        AddTriangle(positions, i0, i1, i2);
                    }
                }
            }
        }

        private void AddTriangle(Point3D[] positions, int i0, int i1, int i2)
        {
            Points.Add(positions[i0]);
            Points.Add(positions[i1]);
            Points.Add(positions[i1]);
            Points.Add(positions[i2]);
            Points.Add(positions[i2]);
            Points.Add(positions[i0]);
        }

        #endregion MakeWireframe

        private Matrix3D _visualToScreen;
        private Matrix3D _screenToVisual;
        private readonly GeometryModel3D _model;
        private readonly MeshGeometry3D _mesh;

        #region MakeNormals

        public void MakeNormals(Model3D model)
        {
            Points.Clear();
            Thickness = 1;
            Color = Colors.SlateBlue;

            if (model == null)
            {
                return;
            }

            Matrix3DStack transform = new Matrix3DStack();
            transform.Push(Matrix3D.Identity);

            NormalsHelper(model, transform);
        }

        private void NormalsHelper(Model3D model, Matrix3DStack matrixStack)
        {
            Transform3D transform = model.Transform;

            if (transform != null && transform != Transform3D.Identity)
            {
                matrixStack.Prepend(model.Transform.Value);
            }

            try
            {
                Model3DGroup group = model as Model3DGroup;

                if (group != null)
                {
                    NormalsHelper(group, matrixStack);
                    return;
                }

                GeometryModel3D geometry = model as GeometryModel3D;

                if (geometry != null)
                {
                    NormalsHelper(geometry, matrixStack);
                    return;
                }
            }
            finally
            {
                if (transform != null && transform != Transform3D.Identity)
                {
                    matrixStack.Pop();
                }
            }
        }

        private void NormalsHelper(Model3DGroup group, Matrix3DStack matrixStack)
        {
            foreach (Model3D child in group.Children)
            {
                NormalsHelper(child, matrixStack);
            }
        }

        private void NormalsHelper(GeometryModel3D model, Matrix3DStack matrixStack)
        {
            Geometry3D geometry = model.Geometry;
            MeshGeometry3D mesh = geometry as MeshGeometry3D;

            if (mesh != null)
            {
                Point3D[] positions = new Point3D[mesh.Positions.Count];
                mesh.Positions.CopyTo(positions, 0);
                //matrixStack.Peek().Transform(positions);

                Vector3D[] normals = new Vector3D[mesh.Normals.Count];
                mesh.Normals.CopyTo(normals, 0);
                new ScaleTransform3D(0.1, 0.1, 0.1).Transform(normals);

                for(int i=0; i<mesh.Positions.Count; i++)
                {
                    Point3D p1 = matrixStack.Peek().Transform(positions[i]);
                    Point3D p2 =
                        matrixStack.Peek().Transform(new Point3D(positions[i].X + normals[i].X,
                                                                 positions[i].Y + normals[i].Y,
                                                                 positions[i].Z + normals[i].Z));

                    Points.Add(p1);
                    Points.Add(p2);
                }
            }
        }

        #endregion

    }
}