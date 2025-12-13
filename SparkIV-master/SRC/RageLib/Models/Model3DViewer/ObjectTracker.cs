
// Modified and adapated from Trackball.cs by Aru, as such it retains 
// the following Microsoft Limited Permissive license.

// Trackball is a camera-oriented control mechanism, whereas ObjectTracker
// is an object-oriented control mechanism that seems to work better when
// looking at single objects.

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
// The following article discusses the mechanics behind this
// trackball implementation: http://viewport3d.com/trackball.htm
//
// Reading the article is not required to use this sample code,
// but skimming it might be useful.
//
//---------------------------------------------------------------------------

using System;
using System.Windows;
using System.Windows.Input;
using System.Windows.Media.Media3D;

namespace RageLib.Models.Model3DViewer
{
    public class ObjectTracker
    {
        private FrameworkElement _eventSource;
        private Point _previousPosition2D;

        private readonly Transform3DGroup _transform;
        private readonly ScaleTransform3D _scale = new ScaleTransform3D();
        private readonly MatrixTransform3D _rotation = new MatrixTransform3D();

        public ObjectTracker()
        {
            _transform = new Transform3DGroup();
            _transform.Children.Add(_scale);
            _transform.Children.Add(_rotation);
        }

        /// <summary>
        ///     A transform to move the camera or scene to the trackball's
        ///     current orientation and scale.
        /// </summary>
        public Transform3D Transform
        {
            get { return _transform; }
        }

        #region Event Handling

        /// <summary>
        ///     The FrameworkElement we listen to for mouse events.
        /// </summary>
        public FrameworkElement EventSource
        {
            get { return _eventSource; }

            set
            {
                if (_eventSource != null)
                {
                    _eventSource.MouseDown -= OnMouseDown;
                    _eventSource.MouseUp -= OnMouseUp;
                    _eventSource.MouseMove -= OnMouseMove;
                }

                _eventSource = value;

                _eventSource.MouseDown += OnMouseDown;
                _eventSource.MouseUp += OnMouseUp;
                _eventSource.MouseMove += OnMouseMove;
            }
        }

        private void OnMouseDown(object sender, MouseEventArgs e)
        {
            Mouse.Capture(EventSource, CaptureMode.Element);
            _previousPosition2D = e.GetPosition(EventSource);
        }

        private void OnMouseUp(object sender, MouseEventArgs e)
        {
            Mouse.Capture(EventSource, CaptureMode.None);
        }

        private void OnMouseMove(object sender, MouseEventArgs e)
        {
            Point currentPosition = e.GetPosition(EventSource);

            // Prefer tracking to zooming if both buttons are pressed.
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                Track(currentPosition);
            }
            else if (e.RightButton == MouseButtonState.Pressed)
            {
                Zoom(currentPosition);
            }

            _previousPosition2D = currentPosition;
        }

        #endregion Event Handling

        private void Track(Point currentPosition)
        {
            Matrix3D current = _rotation.Matrix;

            Matrix3D rotY =
                (new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(0,1,0), currentPosition.X - _previousPosition2D.X)))
                    .Value;

            Matrix3D rotX =
                (new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(1, 0, 0), currentPosition.Y - _previousPosition2D.Y)))
                    .Value;

            Matrix3D newRotation = Matrix3D.Multiply(Matrix3D.Multiply(current, rotY), rotX);

            _rotation.Matrix = newRotation;
        }

        private void Zoom(Point currentPosition)
        {
            double yDelta = _previousPosition2D.Y - currentPosition.Y;

            double scale = Math.Exp(yDelta / 100);    // e^(yDelta/100) is fairly arbitrary.

            _scale.ScaleX *= scale;
            _scale.ScaleY *= scale;
            _scale.ScaleZ *= scale;
        }

    }
}