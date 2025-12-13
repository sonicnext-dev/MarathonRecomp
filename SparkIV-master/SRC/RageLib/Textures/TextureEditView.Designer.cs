/**********************************************************************\

 RageLib - Textures
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

namespace RageLib.Textures
{
    partial class TextureEditView
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TextureEditView));
            this.tsContainer = new System.Windows.Forms.ToolStripContainer();
            this.tssStatus = new System.Windows.Forms.StatusStrip();
            this.tslTexturesInfo = new System.Windows.Forms.ToolStripStatusLabel();
            this.textureView = new RageLib.Textures.TextureView();
            this.tsToolbar = new System.Windows.Forms.ToolStrip();
            this.tsbExport = new System.Windows.Forms.ToolStripButton();
            this.tsbImport = new System.Windows.Forms.ToolStripButton();
            this.tsbSaveClose = new System.Windows.Forms.ToolStripButton();
            this.tsContainer.BottomToolStripPanel.SuspendLayout();
            this.tsContainer.ContentPanel.SuspendLayout();
            this.tsContainer.TopToolStripPanel.SuspendLayout();
            this.tsContainer.SuspendLayout();
            this.tssStatus.SuspendLayout();
            this.tsToolbar.SuspendLayout();
            this.SuspendLayout();
            // 
            // tsContainer
            // 
            // 
            // tsContainer.BottomToolStripPanel
            // 
            this.tsContainer.BottomToolStripPanel.Controls.Add(this.tssStatus);
            // 
            // tsContainer.ContentPanel
            // 
            this.tsContainer.ContentPanel.Controls.Add(this.textureView);
            this.tsContainer.ContentPanel.Size = new System.Drawing.Size(633, 440);
            this.tsContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tsContainer.Location = new System.Drawing.Point(0, 0);
            this.tsContainer.Name = "tsContainer";
            this.tsContainer.Size = new System.Drawing.Size(633, 487);
            this.tsContainer.TabIndex = 5;
            this.tsContainer.Text = "toolStripContainer1";
            // 
            // tsContainer.TopToolStripPanel
            // 
            this.tsContainer.TopToolStripPanel.Controls.Add(this.tsToolbar);
            // 
            // tssStatus
            // 
            this.tssStatus.Dock = System.Windows.Forms.DockStyle.None;
            this.tssStatus.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tslTexturesInfo});
            this.tssStatus.Location = new System.Drawing.Point(0, 0);
            this.tssStatus.Name = "tssStatus";
            this.tssStatus.Size = new System.Drawing.Size(633, 22);
            this.tssStatus.TabIndex = 0;
            // 
            // tslTexturesInfo
            // 
            this.tslTexturesInfo.Name = "tslTexturesInfo";
            this.tslTexturesInfo.Size = new System.Drawing.Size(0, 17);
            // 
            // textureView
            // 
            this.textureView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.textureView.InfoPanelEnabled = false;
            this.textureView.Location = new System.Drawing.Point(0, 0);
            this.textureView.Name = "textureView";
            this.textureView.PreviewImage = null;
            this.textureView.SelectedTexture = null;
            this.textureView.Size = new System.Drawing.Size(633, 440);
            this.textureView.TabIndex = 0;
            // 
            // tsToolbar
            // 
            this.tsToolbar.Dock = System.Windows.Forms.DockStyle.None;
            this.tsToolbar.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.tsToolbar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsbExport,
            this.tsbImport,
            this.tsbSaveClose});
            this.tsToolbar.Location = new System.Drawing.Point(0, 0);
            this.tsToolbar.Name = "tsToolbar";
            this.tsToolbar.Size = new System.Drawing.Size(633, 25);
            this.tsToolbar.Stretch = true;
            this.tsToolbar.TabIndex = 0;
            // 
            // tsbExport
            // 
            this.tsbExport.Image = ((System.Drawing.Image)(resources.GetObject("tsbExport.Image")));
            this.tsbExport.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbExport.Name = "tsbExport";
            this.tsbExport.Size = new System.Drawing.Size(102, 22);
            this.tsbExport.Text = "Export Texture";
            // 
            // tsbImport
            // 
            this.tsbImport.Image = ((System.Drawing.Image)(resources.GetObject("tsbImport.Image")));
            this.tsbImport.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbImport.Name = "tsbImport";
            this.tsbImport.Size = new System.Drawing.Size(105, 22);
            this.tsbImport.Text = "Import Texture";
            // 
            // tsbSaveClose
            // 
            this.tsbSaveClose.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.tsbSaveClose.Image = ((System.Drawing.Image)(resources.GetObject("tsbSaveClose.Image")));
            this.tsbSaveClose.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbSaveClose.Name = "tsbSaveClose";
            this.tsbSaveClose.Size = new System.Drawing.Size(108, 22);
            this.tsbSaveClose.Text = "Save And Close";
            // 
            // TextureEditView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.tsContainer);
            this.Name = "TextureEditView";
            this.Size = new System.Drawing.Size(633, 487);
            this.tsContainer.BottomToolStripPanel.ResumeLayout(false);
            this.tsContainer.BottomToolStripPanel.PerformLayout();
            this.tsContainer.ContentPanel.ResumeLayout(false);
            this.tsContainer.TopToolStripPanel.ResumeLayout(false);
            this.tsContainer.TopToolStripPanel.PerformLayout();
            this.tsContainer.ResumeLayout(false);
            this.tsContainer.PerformLayout();
            this.tssStatus.ResumeLayout(false);
            this.tssStatus.PerformLayout();
            this.tsToolbar.ResumeLayout(false);
            this.tsToolbar.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ToolStripContainer tsContainer;
        private System.Windows.Forms.StatusStrip tssStatus;
        private System.Windows.Forms.ToolStripStatusLabel tslTexturesInfo;
        private TextureView textureView;
        private System.Windows.Forms.ToolStrip tsToolbar;
        private System.Windows.Forms.ToolStripButton tsbExport;
        private System.Windows.Forms.ToolStripButton tsbImport;
        private System.Windows.Forms.ToolStripButton tsbSaveClose;
    }
}
