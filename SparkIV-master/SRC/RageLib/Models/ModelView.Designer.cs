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

using RageLib.Models.Model3DViewer;

namespace RageLib.Models
{
    partial class ModelView
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ModelView));
            this._model3DViewHost = new System.Windows.Forms.Integration.ElementHost();
            this._model3DView = new RageLib.Models.Model3DViewer.Model3DView();
            this.tsContainer = new System.Windows.Forms.ToolStripContainer();
            this.scSplit = new System.Windows.Forms.SplitContainer();
            this.tvNav = new System.Windows.Forms.TreeView();
            this.tsToolbar = new System.Windows.Forms.ToolStrip();
            this.tsbSolid = new System.Windows.Forms.ToolStripButton();
            this.tsbWireframe = new System.Windows.Forms.ToolStripButton();
            this.tss1 = new System.Windows.Forms.ToolStripSeparator();
            this.tsbExport = new System.Windows.Forms.ToolStripButton();
            this.tsContainer.ContentPanel.SuspendLayout();
            this.tsContainer.TopToolStripPanel.SuspendLayout();
            this.tsContainer.SuspendLayout();
            this.scSplit.Panel1.SuspendLayout();
            this.scSplit.Panel2.SuspendLayout();
            this.scSplit.SuspendLayout();
            this.tsToolbar.SuspendLayout();
            this.SuspendLayout();
            // 
            // _model3DViewHost
            // 
            this._model3DViewHost.Dock = System.Windows.Forms.DockStyle.Fill;
            this._model3DViewHost.Location = new System.Drawing.Point(0, 0);
            this._model3DViewHost.Name = "_model3DViewHost";
            this._model3DViewHost.Size = new System.Drawing.Size(471, 435);
            this._model3DViewHost.TabIndex = 0;
            this._model3DViewHost.Child = this._model3DView;
            // 
            // tsContainer
            // 
            // 
            // tsContainer.ContentPanel
            // 
            this.tsContainer.ContentPanel.Controls.Add(this.scSplit);
            this.tsContainer.ContentPanel.Size = new System.Drawing.Size(646, 435);
            this.tsContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tsContainer.Location = new System.Drawing.Point(0, 0);
            this.tsContainer.Name = "tsContainer";
            this.tsContainer.Size = new System.Drawing.Size(646, 460);
            this.tsContainer.TabIndex = 1;
            this.tsContainer.Text = "toolStripContainer1";
            // 
            // tsContainer.TopToolStripPanel
            // 
            this.tsContainer.TopToolStripPanel.Controls.Add(this.tsToolbar);
            // 
            // scSplit
            // 
            this.scSplit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.scSplit.Location = new System.Drawing.Point(0, 0);
            this.scSplit.Name = "scSplit";
            // 
            // scSplit.Panel1
            // 
            this.scSplit.Panel1.Controls.Add(this.tvNav);
            // 
            // scSplit.Panel2
            // 
            this.scSplit.Panel2.Controls.Add(this._model3DViewHost);
            this.scSplit.Size = new System.Drawing.Size(646, 435);
            this.scSplit.SplitterDistance = 171;
            this.scSplit.TabIndex = 0;
            // 
            // tvNav
            // 
            this.tvNav.CheckBoxes = true;
            this.tvNav.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tvNav.HideSelection = false;
            this.tvNav.Location = new System.Drawing.Point(0, 0);
            this.tvNav.Name = "tvNav";
            this.tvNav.Size = new System.Drawing.Size(171, 435);
            this.tvNav.TabIndex = 0;
            this.tvNav.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(this.tvNav_AfterCheck);
            this.tvNav.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvNav_AfterSelect);
            // 
            // tsToolbar
            // 
            this.tsToolbar.Dock = System.Windows.Forms.DockStyle.None;
            this.tsToolbar.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.tsToolbar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsbSolid,
            this.tsbWireframe,
            this.tss1,
            this.tsbExport});
            this.tsToolbar.Location = new System.Drawing.Point(0, 0);
            this.tsToolbar.Name = "tsToolbar";
            this.tsToolbar.Size = new System.Drawing.Size(646, 25);
            this.tsToolbar.Stretch = true;
            this.tsToolbar.TabIndex = 0;
            // 
            // tsbSolid
            // 
            this.tsbSolid.Checked = true;
            this.tsbSolid.CheckOnClick = true;
            this.tsbSolid.CheckState = System.Windows.Forms.CheckState.Checked;
            this.tsbSolid.Image = ((System.Drawing.Image)(resources.GetObject("tsbSolid.Image")));
            this.tsbSolid.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbSolid.Name = "tsbSolid";
            this.tsbSolid.Size = new System.Drawing.Size(53, 22);
            this.tsbSolid.Text = "Solid";
            this.tsbSolid.CheckedChanged += new System.EventHandler(this.tsbSolid_CheckedChanged);
            // 
            // tsbWireframe
            // 
            this.tsbWireframe.CheckOnClick = true;
            this.tsbWireframe.Image = ((System.Drawing.Image)(resources.GetObject("tsbWireframe.Image")));
            this.tsbWireframe.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbWireframe.Name = "tsbWireframe";
            this.tsbWireframe.Size = new System.Drawing.Size(82, 22);
            this.tsbWireframe.Text = "Wireframe";
            this.tsbWireframe.Click += new System.EventHandler(this.tsbWireframe_Click);
            // 
            // tss1
            // 
            this.tss1.Name = "tss1";
            this.tss1.Size = new System.Drawing.Size(6, 25);
            // 
            // tsbExport
            // 
            this.tsbExport.Image = ((System.Drawing.Image)(resources.GetObject("tsbExport.Image")));
            this.tsbExport.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbExport.Name = "tsbExport";
            this.tsbExport.Size = new System.Drawing.Size(60, 22);
            this.tsbExport.Text = "Export";
            // 
            // ModelView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.tsContainer);
            this.Name = "ModelView";
            this.Size = new System.Drawing.Size(646, 460);
            this.tsContainer.ContentPanel.ResumeLayout(false);
            this.tsContainer.TopToolStripPanel.ResumeLayout(false);
            this.tsContainer.TopToolStripPanel.PerformLayout();
            this.tsContainer.ResumeLayout(false);
            this.tsContainer.PerformLayout();
            this.scSplit.Panel1.ResumeLayout(false);
            this.scSplit.Panel2.ResumeLayout(false);
            this.scSplit.ResumeLayout(false);
            this.tsToolbar.ResumeLayout(false);
            this.tsToolbar.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Integration.ElementHost _model3DViewHost;
        private Model3DView _model3DView;
        private System.Windows.Forms.ToolStripContainer tsContainer;
        private System.Windows.Forms.SplitContainer scSplit;
        private System.Windows.Forms.TreeView tvNav;
        private System.Windows.Forms.ToolStrip tsToolbar;
        private System.Windows.Forms.ToolStripButton tsbSolid;
        private System.Windows.Forms.ToolStripButton tsbWireframe;
        private System.Windows.Forms.ToolStripSeparator tss1;
        private System.Windows.Forms.ToolStripButton tsbExport;
    }
}
