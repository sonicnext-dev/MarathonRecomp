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

using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Media3D;

namespace RageLib.Models.Model3DViewer
{
    /// <summary>
    /// Interaction logic for Model3DView.xaml
    /// </summary>
    public partial class Model3DView : UserControl
    {
        private ObjectTracker _tracker = new ObjectTracker();
        private ScreenSpaceLines3D _lines = new ScreenSpaceLines3D();
        private RenderMode _renderMode;
        private Model3D _model;

        public Model3DView()
        {
            InitializeComponent();

            _lines.Transform = _tracker.Transform;
            MainViewport.Children.Add(_lines);

            Root.Transform = _tracker.Transform;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            // Viewport3Ds only raise events when the mouse is over the rendered 3D geometry.
            // In order to capture events whenever the mouse is over the client are we use a
            // same sized transparent Border positioned on top of the Viewport3D.
            _tracker.EventSource = CaptureBorder;
        }

        public RenderMode RenderMode
        {
            get { return _renderMode; }
            set
            {
                _renderMode = value;
                Model = _model;
            }
        }

        public Model3D Model
        {
            set
            {
                _model = value;

                if (value == null)
                {
                    _lines.Points.Clear();
                    Root.Content = null;
                    return;
                }

                var group = new Model3DGroup();
                group.Children.Add(value);

                var transformGroup = new Transform3DGroup();
                transformGroup.Children.Add(new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(1, 0, 0), -90)));
                transformGroup.Children.Add(new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(0, 1, 0), 180)));
                group.Transform = transformGroup;

                if (RenderMode == RenderMode.Solid)
                {
                    _lines.Points.Clear();
                    Root.Content = group;
                }
                else if (RenderMode == RenderMode.SolidNormals)
                {
                    _lines.MakeNormals(group);
                    Root.Content = group;
                }
                else if (RenderMode == RenderMode.Wireframe)
                {
                    _lines.MakeWireframe(group);
                    Root.Content = null;
                }
            }
        }
    }
}