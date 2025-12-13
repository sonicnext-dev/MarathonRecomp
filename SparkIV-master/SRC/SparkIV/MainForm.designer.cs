/**********************************************************************\

 Spark IV
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

namespace SparkIV
{
    partial class MainForm
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.tvDir = new System.Windows.Forms.TreeView();
            this.tsContainer = new System.Windows.Forms.ToolStripContainer();
            this.splitContainer = new System.Windows.Forms.SplitContainer();
            this.lvFiles = new System.Windows.Forms.ListView();
            this.lvcName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.lvcSize = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.lvcResource = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
            this.tsToolbar = new System.Windows.Forms.ToolStrip();
            this.toolStripGTAIV = new System.Windows.Forms.ToolStripButton();
            this.toolStripEFLC = new System.Windows.Forms.ToolStripButton();
            this.tsbOpen = new System.Windows.Forms.ToolStripButton();
            this.tsbSave = new System.Windows.Forms.ToolStripButton();
            this.tsbRebuild = new System.Windows.Forms.ToolStripButton();
            this.tss1 = new System.Windows.Forms.ToolStripSeparator();
            this.tsbExportSelected = new System.Windows.Forms.ToolStripButton();
            this.tsbExportAll = new System.Windows.Forms.ToolStripButton();
            this.tsbImport = new System.Windows.Forms.ToolStripButton();
            this.tslAbout = new System.Windows.Forms.ToolStripLabel();
            this.tss2 = new System.Windows.Forms.ToolStripSeparator();
            this.tsbPreview = new System.Windows.Forms.ToolStripButton();
            this.tsbEdit = new System.Windows.Forms.ToolStripButton();
            this.tslFilter = new System.Windows.Forms.ToolStripLabel();
            this.tstFilterBox = new System.Windows.Forms.ToolStripTextBox();
            this.tsContainer.ContentPanel.SuspendLayout();
            this.tsContainer.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer)).BeginInit();
            this.splitContainer.Panel1.SuspendLayout();
            this.splitContainer.Panel2.SuspendLayout();
            this.splitContainer.SuspendLayout();
            this.toolStripContainer1.ContentPanel.SuspendLayout();
            this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
            this.toolStripContainer1.SuspendLayout();
            this.tsToolbar.SuspendLayout();
            this.SuspendLayout();
            // 
            // tvDir
            // 
            this.tvDir.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tvDir.HideSelection = false;
            this.tvDir.Location = new System.Drawing.Point(0, 0);
            this.tvDir.Name = "tvDir";
            this.tvDir.Size = new System.Drawing.Size(192, 100);
            this.tvDir.TabIndex = 0;
            this.tvDir.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.tvDir_AfterSelect);
            // 
            // tsContainer
            // 
            // 
            // tsContainer.ContentPanel
            // 
            this.tsContainer.ContentPanel.Controls.Add(this.splitContainer);
            this.tsContainer.ContentPanel.Size = new System.Drawing.Size(716, 438);
            this.tsContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tsContainer.Location = new System.Drawing.Point(0, 0);
            this.tsContainer.Name = "tsContainer";
            this.tsContainer.Size = new System.Drawing.Size(716, 463);
            this.tsContainer.TabIndex = 1;
            this.tsContainer.Text = "toolStripContainer1";
            // 
            // splitContainer
            // 
            this.splitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer.Location = new System.Drawing.Point(0, 0);
            this.splitContainer.Name = "splitContainer";
            // 
            // splitContainer.Panel1
            // 
            this.splitContainer.Panel1.Controls.Add(this.tvDir);
            this.splitContainer.Panel1Collapsed = true;
            // 
            // splitContainer.Panel2
            // 
            this.splitContainer.Panel2.Controls.Add(this.lvFiles);
            this.splitContainer.Size = new System.Drawing.Size(716, 438);
            this.splitContainer.SplitterDistance = 192;
            this.splitContainer.TabIndex = 2;
            // 
            // lvFiles
            // 
            this.lvFiles.AllowDrop = true;
            this.lvFiles.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.lvcName,
            this.lvcSize,
            this.lvcResource});
            this.lvFiles.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lvFiles.FullRowSelect = true;
            this.lvFiles.HideSelection = false;
            this.lvFiles.Location = new System.Drawing.Point(0, 0);
            this.lvFiles.Name = "lvFiles";
            this.lvFiles.Size = new System.Drawing.Size(716, 438);
            this.lvFiles.TabIndex = 1;
            this.lvFiles.UseCompatibleStateImageBehavior = false;
            this.lvFiles.View = System.Windows.Forms.View.Details;
            this.lvFiles.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.lvFiles_ColumnClick);
            this.lvFiles.SelectedIndexChanged += new System.EventHandler(this.lvFiles_SelectedIndexChanged);
            this.lvFiles.KeyDown += new System.Windows.Forms.KeyEventHandler(this.lvFiles_KeyDown);
            this.lvFiles.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.lvFiles_MouseDoubleClick);
            // 
            // lvcName
            // 
            this.lvcName.Text = "Name";
            this.lvcName.Width = 280;
            // 
            // lvcSize
            // 
            this.lvcSize.Text = "Size";
            this.lvcSize.Width = 80;
            // 
            // lvcResource
            // 
            this.lvcResource.Text = "Resource";
            this.lvcResource.Width = 100;
            // 
            // toolStripContainer1
            // 
            // 
            // toolStripContainer1.ContentPanel
            // 
            this.toolStripContainer1.ContentPanel.AutoScroll = true;
            this.toolStripContainer1.ContentPanel.Controls.Add(this.tsContainer);
            this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(716, 463);
            this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.toolStripContainer1.Location = new System.Drawing.Point(0, 0);
            this.toolStripContainer1.Name = "toolStripContainer1";
            this.toolStripContainer1.Size = new System.Drawing.Size(716, 517);
            this.toolStripContainer1.TabIndex = 2;
            this.toolStripContainer1.Text = "toolStripContainer1";
            // 
            // toolStripContainer1.TopToolStripPanel
            // 
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.tsToolbar);
            // 
            // tsToolbar
            // 
            this.tsToolbar.CanOverflow = false;
            this.tsToolbar.Dock = System.Windows.Forms.DockStyle.None;
            this.tsToolbar.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.tsToolbar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripGTAIV,
            this.toolStripEFLC,
            this.tsbOpen,
            this.tsbSave,
            this.tsbRebuild,
            this.tss1,
            this.tsbExportSelected,
            this.tsbExportAll,
            this.tsbImport,
            this.tslAbout,
            this.tss2,
            this.tsbPreview,
            this.tsbEdit,
            this.tslFilter,
            this.tstFilterBox});
            this.tsToolbar.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.HorizontalStackWithOverflow;
            this.tsToolbar.Location = new System.Drawing.Point(0, 0);
            this.tsToolbar.Name = "tsToolbar";
            this.tsToolbar.Size = new System.Drawing.Size(716, 54);
            this.tsToolbar.Stretch = true;
            this.tsToolbar.TabIndex = 0;
            // 
            // toolStripGTAIV
            // 
            this.toolStripGTAIV.Image = ((System.Drawing.Image)(resources.GetObject("toolStripGTAIV.Image")));
            this.toolStripGTAIV.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolStripGTAIV.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripGTAIV.Name = "toolStripGTAIV";
            this.toolStripGTAIV.Size = new System.Drawing.Size(43, 51);
            this.toolStripGTAIV.Text = "&GTAIV";
            this.toolStripGTAIV.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripGTAIV.Click += new System.EventHandler(this.toolStripGTAIV_Click);
            // 
            // toolStripEFLC
            // 
            this.toolStripEFLC.Image = ((System.Drawing.Image)(resources.GetObject("toolStripEFLC.Image")));
            this.toolStripEFLC.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolStripEFLC.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripEFLC.Name = "toolStripEFLC";
            this.toolStripEFLC.Size = new System.Drawing.Size(39, 51);
            this.toolStripEFLC.Text = "E&FLC";
            this.toolStripEFLC.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.toolStripEFLC.Click += new System.EventHandler(this.toolStripEFLC_Click);
            // 
            // tsbOpen
            // 
            this.tsbOpen.Image = ((System.Drawing.Image)(resources.GetObject("tsbOpen.Image")));
            this.tsbOpen.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tsbOpen.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbOpen.Name = "tsbOpen";
            this.tsbOpen.Size = new System.Drawing.Size(40, 51);
            this.tsbOpen.Text = "&Open";
            this.tsbOpen.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.tsbOpen.Click += new System.EventHandler(this.tsbOpen_Click);
            // 
            // tsbSave
            // 
            this.tsbSave.Image = ((System.Drawing.Image)(resources.GetObject("tsbSave.Image")));
            this.tsbSave.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tsbSave.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbSave.Name = "tsbSave";
            this.tsbSave.Size = new System.Drawing.Size(36, 51);
            this.tsbSave.Text = "&Save";
            this.tsbSave.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.tsbSave.Click += new System.EventHandler(this.tsbSave_Click);
            // 
            // tsbRebuild
            // 
            this.tsbRebuild.Image = ((System.Drawing.Image)(resources.GetObject("tsbRebuild.Image")));
            this.tsbRebuild.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tsbRebuild.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbRebuild.Name = "tsbRebuild";
            this.tsbRebuild.Size = new System.Drawing.Size(51, 51);
            this.tsbRebuild.Text = "&Rebuild";
            this.tsbRebuild.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.tsbRebuild.Click += new System.EventHandler(this.tsbRebuild_Click);
            // 
            // tss1
            // 
            this.tss1.Name = "tss1";
            this.tss1.Size = new System.Drawing.Size(6, 54);
            // 
            // tsbExportSelected
            // 
            this.tsbExportSelected.Image = ((System.Drawing.Image)(resources.GetObject("tsbExportSelected.Image")));
            this.tsbExportSelected.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tsbExportSelected.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbExportSelected.Name = "tsbExportSelected";
            this.tsbExportSelected.Size = new System.Drawing.Size(44, 51);
            this.tsbExportSelected.Text = "&Export";
            this.tsbExportSelected.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.tsbExportSelected.Click += new System.EventHandler(this.tsbExportSelected_Click);
            // 
            // tsbExportAll
            // 
            this.tsbExportAll.Image = ((System.Drawing.Image)(resources.GetObject("tsbExportAll.Image")));
            this.tsbExportAll.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tsbExportAll.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbExportAll.Name = "tsbExportAll";
            this.tsbExportAll.Size = new System.Drawing.Size(61, 51);
            this.tsbExportAll.Text = "Export &All";
            this.tsbExportAll.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.tsbExportAll.Click += new System.EventHandler(this.tsbExportAll_Click);
            // 
            // tsbImport
            // 
            this.tsbImport.Image = ((System.Drawing.Image)(resources.GetObject("tsbImport.Image")));
            this.tsbImport.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tsbImport.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbImport.Name = "tsbImport";
            this.tsbImport.Size = new System.Drawing.Size(47, 51);
            this.tsbImport.Text = "&Import";
            this.tsbImport.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.tsbImport.Click += new System.EventHandler(this.tsbImport_Click);
            // 
            // tslAbout
            // 
            this.tslAbout.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.tslAbout.BackColor = System.Drawing.SystemColors.Control;
            this.tslAbout.ForeColor = System.Drawing.Color.SlateGray;
            this.tslAbout.Name = "tslAbout";
            this.tslAbout.Size = new System.Drawing.Size(49, 51);
            this.tslAbout.Text = "Spark IV";
            this.tslAbout.ToolTipText = "Click to check for new updates.";
            this.tslAbout.Click += new System.EventHandler(this.tslAbout_Click);
            // 
            // tss2
            // 
            this.tss2.Name = "tss2";
            this.tss2.Size = new System.Drawing.Size(6, 54);
            // 
            // tsbPreview
            // 
            this.tsbPreview.Image = ((System.Drawing.Image)(resources.GetObject("tsbPreview.Image")));
            this.tsbPreview.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tsbPreview.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbPreview.Name = "tsbPreview";
            this.tsbPreview.Size = new System.Drawing.Size(36, 51);
            this.tsbPreview.Text = "&View";
            this.tsbPreview.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.tsbPreview.Click += new System.EventHandler(this.tsbPreview_Click);
            // 
            // tsbEdit
            // 
            this.tsbEdit.Image = ((System.Drawing.Image)(resources.GetObject("tsbEdit.Image")));
            this.tsbEdit.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.tsbEdit.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbEdit.Name = "tsbEdit";
            this.tsbEdit.Size = new System.Drawing.Size(36, 51);
            this.tsbEdit.Text = "&Edit";
            this.tsbEdit.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.tsbEdit.Click += new System.EventHandler(this.tsbEdit_Click);
            // 
            // tslFilter
            // 
            this.tslFilter.Name = "tslFilter";
            this.tslFilter.Size = new System.Drawing.Size(36, 51);
            this.tslFilter.Text = "Fil&ter:";
            // 
            // tstFilterBox
            // 
            this.tstFilterBox.Name = "tstFilterBox";
            this.tstFilterBox.Size = new System.Drawing.Size(100, 54);
            this.tstFilterBox.ToolTipText = "Type all or part of a file name.\r\nSearch is case-sensitive.";
            this.tstFilterBox.Click += new System.EventHandler(this.tstFilterBox_Click);
            this.tstFilterBox.TextChanged += new System.EventHandler(this.tstFilterBox_TextChanged);
            // 
            // MainForm
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(716, 517);
            this.Controls.Add(this.toolStripContainer1);
            this.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Spark IV (Beta)";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.tsContainer.ContentPanel.ResumeLayout(false);
            this.tsContainer.ResumeLayout(false);
            this.tsContainer.PerformLayout();
            this.splitContainer.Panel1.ResumeLayout(false);
            this.splitContainer.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer)).EndInit();
            this.splitContainer.ResumeLayout(false);
            this.toolStripContainer1.ContentPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.PerformLayout();
            this.toolStripContainer1.ResumeLayout(false);
            this.toolStripContainer1.PerformLayout();
            this.tsToolbar.ResumeLayout(false);
            this.tsToolbar.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView tvDir;
        private System.Windows.Forms.ToolStripContainer tsContainer;
        private System.Windows.Forms.SplitContainer splitContainer;
        private System.Windows.Forms.ListView lvFiles;
        private System.Windows.Forms.ColumnHeader lvcName;
        private System.Windows.Forms.ColumnHeader lvcSize;
        private System.Windows.Forms.ColumnHeader lvcResource;
        private System.Windows.Forms.ToolStripContainer toolStripContainer1;
        private System.Windows.Forms.ToolStrip tsToolbar;
        private System.Windows.Forms.ToolStripButton toolStripGTAIV;
        private System.Windows.Forms.ToolStripButton toolStripEFLC;
        private System.Windows.Forms.ToolStripButton tsbOpen;
        private System.Windows.Forms.ToolStripButton tsbSave;
        private System.Windows.Forms.ToolStripButton tsbRebuild;
        private System.Windows.Forms.ToolStripSeparator tss1;
        private System.Windows.Forms.ToolStripButton tsbExportSelected;
        private System.Windows.Forms.ToolStripButton tsbExportAll;
        private System.Windows.Forms.ToolStripButton tsbImport;
        private System.Windows.Forms.ToolStripLabel tslAbout;
        private System.Windows.Forms.ToolStripSeparator tss2;
        private System.Windows.Forms.ToolStripButton tsbPreview;
        private System.Windows.Forms.ToolStripButton tsbEdit;
        private System.Windows.Forms.ToolStripLabel tslFilter;
        private System.Windows.Forms.ToolStripTextBox tstFilterBox;
    }
}