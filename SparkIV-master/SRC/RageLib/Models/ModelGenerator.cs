/**********************************************************************\

 RageLib
 Copyright (C) 2008  Arushan/Aru <oneforaru at gmail.com>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

\**********************************************************************/

using System;
using System.Drawing;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using RageLib.Models.Data;
using RageLib.Models.Resource;
using RageLib.Models.Resource.Shaders;
using RageLib.Textures;
using Brush=System.Windows.Media.Brush;
using Material=System.Windows.Media.Media3D.Material;
using Point=System.Windows.Point;

namespace RageLib.Models
{
    internal static class ModelGenerator
    {
        private static Texture FindTexture(TextureFile textures, string name)
        {
            if (textures == null)
            {
                return null;
            }

            var textureObj = textures.FindTextureByName(name);
            return textureObj;
        }

        /*
        internal static Image CreateUVMapImage(DrawableModel drawable)
        {
            var bmp = new Bitmap(512, 512);
            var g = Graphics.FromImage(bmp);
            var pen = new System.Drawing.Pen(Color.Red);

            foreach (var geometryInfo in drawable.ModelCollection)
            {
                foreach (var dataInfo in geometryInfo.Geometries)
                {
                    for (var i = 0; i < dataInfo.FaceCount; i++)
                    {
                        var i1 = (dataInfo.IndexBuffer.IndexData[i * 3 + 0]);
                        var i2 = (dataInfo.IndexBuffer.IndexData[i * 3 + 1]);
                        var i3 = (dataInfo.IndexBuffer.IndexData[i * 3 + 2]);

                        var v1 = dataInfo.VertexBuffer.VertexData[i1];
                        var v2 = dataInfo.VertexBuffer.VertexData[i2];
                        var v3 = dataInfo.VertexBuffer.VertexData[i3];

                        g.DrawLine(pen, v1.TextureU * bmp.Width, v1.TextureV * bmp.Height, v2.TextureU * bmp.Width, v2.TextureV * bmp.Height);
                        g.DrawLine(pen, v1.TextureU * bmp.Width, v1.TextureV * bmp.Height, v3.TextureU * bmp.Width, v3.TextureV * bmp.Height);
                        g.DrawLine(pen, v2.TextureU * bmp.Width, v2.TextureV * bmp.Height, v3.TextureU * bmp.Width, v3.TextureV * bmp.Height);
                    }
                }
            }

            g.Dispose();

            return bmp;
        }
         */

        internal static ModelNode GenerateModel(FragTypeModel fragTypeModel, TextureFile[] textures)
        {
            var fragTypeGroup = new Model3DGroup();
            var fragTypeNode = new ModelNode { DataModel = fragTypeModel, Model3D = fragTypeGroup, Name = "FragType", NoCount = true };

            var parentDrawableNode = GenerateModel(fragTypeModel.Drawable, textures);
            parentDrawableNode.NoCount = false;
            fragTypeGroup.Children.Add(parentDrawableNode.Model3D);
            fragTypeNode.Children.Add(parentDrawableNode);

            foreach (var fragTypeChild in fragTypeModel.Children)
            {
                if (fragTypeChild.Drawable != null && fragTypeChild.Drawable.ModelCollection.Length > 0)
                {
                    var childDrawableNode = GenerateModel(fragTypeChild.Drawable, textures);
                    childDrawableNode.NoCount = false;
                    fragTypeGroup.Children.Add(childDrawableNode.Model3D);
                    fragTypeNode.Children.Add(childDrawableNode);                    
                }
            }

            return fragTypeNode;
        }

        internal static ModelNode GenerateModel(DrawableModelDictionary drawableModelDictionary, TextureFile[] textures)
        {
            var dictionaryTypeGroup = new Model3DGroup();
            var dictionaryTypeNode = new ModelNode { DataModel = drawableModelDictionary, Model3D = dictionaryTypeGroup, Name = "Dictionary", NoCount = true };
            foreach (var entry in drawableModelDictionary.Entries)
            {
                var drawableNode = GenerateModel(entry, textures);
                drawableNode.NoCount = false;
                dictionaryTypeGroup.Children.Add(drawableNode.Model3D);
                dictionaryTypeNode.Children.Add(drawableNode);                    
            }
            return dictionaryTypeNode;
        }

        internal static ModelNode GenerateModel(DrawableModel drawableModel, TextureFile[] textures)
        {
            return GenerateModel(new Drawable(drawableModel), textures);
        }

        internal static ModelNode GenerateModel(Drawable drawable, TextureFile[] textures)
        {
            var random = new Random();

            var materials = new Material[drawable.Materials.Count];
            for (int i = 0; i < materials.Length; i++)
            {
                Brush brush =
                    new SolidColorBrush(System.Windows.Media.Color.FromArgb(255, (byte)random.Next(0, 255),
                                                                            (byte)random.Next(0, 255),
                                                                            (byte)random.Next(0, 255)));

                var drawableMat = drawable.Materials[i];
                var texture = drawableMat.Parameters[(int)ParamNameHash.Texture] as MaterialParamTexture;
                if (texture != null)
                {
                    // 1. Try looking in the embedded texture file (if any)
                    var textureObj = FindTexture(drawable.AttachedTexture, texture.TextureName);

                    // 2. Try looking in any attached external texture dictionaries
                    if (textureObj == null)
                    {
                        foreach (var file in textures)
                        {
                            textureObj = FindTexture(file, texture.TextureName);
                            if (textureObj != null)
                            {
                                break;
                            }
                        }
                    }

                    // Generate a brush if we were successful
                    if (textureObj != null)
                    {
                        var bitmap = textureObj.Decode() as Bitmap;

                        var bitmapSource = Imaging.CreateBitmapSourceFromHBitmap(
                            bitmap.GetHbitmap(),
                            IntPtr.Zero,
                            Int32Rect.Empty,
                            BitmapSizeOptions.FromEmptyOptions());

                        // For memory leak work around
                        bitmapSource.Freeze();

                        brush = new ImageBrush(bitmapSource);
                        (brush as ImageBrush).ViewportUnits = BrushMappingMode.Absolute;
                        (brush as ImageBrush).TileMode = TileMode.Tile;

                        bitmap.Dispose();
                    }
                }

                materials[i] = new DiffuseMaterial(brush);
            }

            var drawableModelGroup = new Model3DGroup();
            var drawableModelNode = new ModelNode {DataModel = drawable, Model3D = drawableModelGroup, Name = "Drawable", NoCount = true};
            foreach (var model in drawable.Models)
            {
                var modelGroup = new Model3DGroup();

                var modelNode = new ModelNode {DataModel = model, Model3D = modelGroup, Name = "Model"};
                drawableModelNode.Children.Add(modelNode);

                foreach (var geometry in model.Geometries)
                {
                    var geometryIndex = 0;
                    var geometryGroup = new Model3DGroup();

                    var geometryNode = new ModelNode { DataModel = geometry, Model3D = geometryGroup, Name = "Geometry" };
                    modelNode.Children.Add(geometryNode);

                    foreach (var mesh in geometry.Meshes)
                    {
                        var mesh3D = new MeshGeometry3D();

                        var meshNode = new ModelNode { DataModel = mesh, Model3D = null, Name = "Mesh" };
                        geometryNode.Children.Add(meshNode);

                        Data.Vertex[] vertices = mesh.DecodeVertexData();
                        foreach (var vertex in vertices)
                        {
                            mesh3D.Positions.Add(new Point3D(vertex.Position.X, vertex.Position.Y, vertex.Position.Z));
                            if (mesh.VertexHasNormal)
                            {
                                mesh3D.Normals.Add(new Vector3D(vertex.Normal.X, vertex.Normal.Y, vertex.Normal.Z));
                            }

                            if (mesh.VertexHasTexture)
                            {
                                mesh3D.TextureCoordinates.Add(new Point(vertex.TextureCoordinates[0].X, vertex.TextureCoordinates[0].Y));
                            }
                        }

                        ushort[] indices = mesh.DecodeIndexData();
                        for (int i = 0; i < mesh.FaceCount; i++)
                        {
                            mesh3D.TriangleIndices.Add(indices[i * 3 + 0]);
                            mesh3D.TriangleIndices.Add(indices[i * 3 + 1]);
                            mesh3D.TriangleIndices.Add(indices[i * 3 + 2]);
                        }

                        var material = materials[geometry.Meshes[geometryIndex].MaterialIndex];
                        var model3D = new GeometryModel3D(mesh3D, material);

                        geometryGroup.Children.Add(model3D);
                        meshNode.Model3D = model3D;

                        geometryIndex++;
                    }
                    modelGroup.Children.Add(geometryGroup);
                }
                drawableModelGroup.Children.Add(modelGroup);
            }
            return drawableModelNode;
        }
    }
}
