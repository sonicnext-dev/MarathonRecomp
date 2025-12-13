/**********************************************************************\

 RageLib - Models
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
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using System.Windows.Media.Media3D;
using RageLib.Textures;

namespace RageLib.Models
{
    public class ModelViewController
    {
        private readonly ModelView _view;
        private IModelFile _modelFile;
        private ModelNode _rootModelNode;
        private TextureFile[] _textureFiles;
        private string _workingDirectory;

        public ModelViewController(ModelView view)
        {
            _view = view;

            _view.ExportClicked += View_ExportClicked;
            _view.Disposed += View_Disposed;
            _view.RefreshDisplayModel += View_RefreshDisplayModel;
        }

        private void View_RefreshDisplayModel(object sender, EventArgs e)
        {
            List<ModelNode> viewableNodes = new List<ModelNode>();
            FindViewableNodes( _rootModelNode, viewableNodes );

            if (!_view.SelectedNavigationModel.Selected)
            {
                viewableNodes.Add(_view.SelectedNavigationModel);
            }

            Model3DGroup group = new Model3DGroup();
            foreach (var node in viewableNodes)
            {
                group.Children.Add(node.Model3D);
            }

            _view.DisplayModel = group;
        }

        private void FindViewableNodes(ModelNode node, List<ModelNode> viewableNodes )
        {
            if (node.Selected)
            {
                viewableNodes.Add(node);
            }
            else
            {
                foreach (var child in node.Children)
                {
                    FindViewableNodes(child, viewableNodes);
                }
            }
        }

        public TextureFile[] TextureFiles
        {
            get { return _textureFiles; }
            set { _textureFiles = value; }
        }

        public IModelFile ModelFile
        {
            get { return _modelFile; }
            set
            {
                _modelFile = value;
                UpdateView();
            }
        }

        private void UpdateView()
        {
            if (_modelFile != null)
            {
                _rootModelNode = _modelFile.GetModel(_textureFiles);
                _view.NavigationModel = _rootModelNode;
            }
            else
            {
                _rootModelNode = null;
                _view.NavigationModel = null;
                _view.DisplayModel = null;
            }
        }

        private void View_ExportClicked(object sender, EventArgs e)
        {
            var model = _rootModelNode;
            if (model != null)
            {
                var sfd = new SaveFileDialog
                {
                    AddExtension = true,
                    OverwritePrompt = true,
                    Title = "Export Model",
                    Filter = Export.ExportFactory.GenerateFilterString(),
                    InitialDirectory = _workingDirectory,
                };

                if (sfd.ShowDialog() == DialogResult.OK && sfd.FilterIndex > 0)
                {
                    Export.IExporter exporter = Export.ExportFactory.GetExporter(sfd.FilterIndex - 1);
                    exporter.Export( model, sfd.FileName );

                    _workingDirectory = new FileInfo(sfd.FileName).Directory.FullName;
                }
            }
        }

        private void View_Disposed(object sender, EventArgs e)
        {
            if (TextureFiles != null)
            {
                foreach (var file in _textureFiles)
                {
                    file.Dispose();
                }
                TextureFiles = null;
            }
            
            if (ModelFile != null)
            {
                ModelFile.Dispose();
                ModelFile = null;
            }
        }
    }
}
