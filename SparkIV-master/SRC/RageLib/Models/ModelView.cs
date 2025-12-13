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
using System.Collections.Generic;
using System.Windows.Forms;
using System.Windows.Media.Media3D;
using RageLib.Models.Model3DViewer;
using UserControl=System.Windows.Forms.UserControl;

namespace RageLib.Models
{
    public partial class ModelView : UserControl
    {
        public event EventHandler RefreshDisplayModel;

        public ModelView()
        {
            InitializeComponent();
            _model3DView.RenderMode = RenderMode.Solid;
        }

        public RenderMode RenderMode
        {
            get { return _model3DView.RenderMode;  }
            set
            {
                if (_model3DView.RenderMode != value)
                {
                    _model3DView.RenderMode = value;
                    tsbWireframe.Checked = value == RenderMode.Wireframe;
                    tsbSolid.Checked = value == RenderMode.Solid;
                }
            }
        }

        public ModelNode SelectedNavigationModel
        {
            get
            {
                return tvNav.SelectedNode.Tag as ModelNode;
            }
        }

        public ModelNode NavigationModel
        {
            set
            {
                UpdateTreeView(value);
            }
        }

        public Model3D DisplayModel
        {
            set
            {
                _model3DView.Model = value;
            }
        }

        private void UpdateTreeView(ModelNode model)
        {
            if (!tvNav.IsDisposed)
            {
                tvNav.Nodes.Clear();
            }

            if (model != null)
            {
                var node = tvNav.Nodes.Add(model.Name);
                node.Tag = model;

                if (model.Children.Count > 0)
                {
                    AddModelGroup(model.Children, node);
                }
            }
        }

        private void AddModelGroup(List<ModelNode> group, TreeNode node)
        {
            int index = 1;
            foreach (var child in group)
            {
                TreeNode newNode = node.Nodes.Add(child.Name + " " + index);
                newNode.Tag = child;

                if (child.Children.Count > 0)
                {
                    AddModelGroup(child.Children, newNode);
                }

                index++;
            }
        }

        public event EventHandler ExportClicked
        {
            add { tsbExport.Click += value; }
            remove { tsbExport.Click -= value; }
        }

        private void tsbSolid_CheckedChanged(object sender, EventArgs e)
        {
            if (tsbSolid.Checked)
            {
                RenderMode = RenderMode.Solid;
            }
        }

        private void tsbWireframe_Click(object sender, EventArgs e)
        {
            if (tsbWireframe.Checked)
            {
                RenderMode = RenderMode.Wireframe;
            }
        }

        private void tvNav_AfterCheck(object sender, TreeViewEventArgs e)
        {
            ((ModelNode) e.Node.Tag).Selected = e.Node.Checked;

            if (RefreshDisplayModel != null)
            {
                RefreshDisplayModel(this, null);
            }
        }

        private void tvNav_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (RefreshDisplayModel != null)
            {
                RefreshDisplayModel(this, null);
            }
        }

    }
}
