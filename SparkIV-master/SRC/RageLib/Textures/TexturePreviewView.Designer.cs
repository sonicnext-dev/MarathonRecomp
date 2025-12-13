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
    partial class TexturePreviewView
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TexturePreviewView));
            this.tsContainer = new System.Windows.Forms.ToolStripContainer();
            this.tssStatus = new System.Windows.Forms.StatusStrip();
            this.tslTexturesInfo = new System.Windows.Forms.ToolStripStatusLabel();
            this.textureView = new RageLib.Textures.TextureView();
            this.tsToolbar = new System.Windows.Forms.ToolStrip();
            this.tsbSave = new System.Windows.Forms.ToolStripButton();
            this.tsbSaveAll = new System.Windows.Forms.ToolStripButton();
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
            this.tsContainer.TabIndex = 4;
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
            this.textureView.Location = new System.Drawing.Point(0, 0);
            this.textureView.Name = "textureView";
            this.textureView.Size = new System.Drawing.Size(633, 440);
            this.textureView.TabIndex = 0;
            // 
            // tsToolbar
            // 
            this.tsToolbar.Dock = System.Windows.Forms.DockStyle.None;
            this.tsToolbar.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.tsToolbar.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsbSave,
            this.tsbSaveAll});
            this.tsToolbar.Location = new System.Drawing.Point(0, 0);
            this.tsToolbar.Name = "tsToolbar";
            this.tsToolbar.Size = new System.Drawing.Size(633, 25);
            this.tsToolbar.Stretch = true;
            this.tsToolbar.TabIndex = 0;
            // 
            // tsbSave
            // 
            this.tsbSave.Image = ((System.Drawing.Image)(resources.GetObject("tsbSave.Image")));
            this.tsbSave.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbSave.Name = "tsbSave";
            this.tsbSave.Size = new System.Drawing.Size(93, 22);
            this.tsbSave.Text = "Save Texture";
            // 
            // tsbSaveAll
            // 
            this.tsbSaveAll.Image = ((System.Drawing.Image)(resources.GetObject("tsbSaveAll.Image")));
            this.tsbSaveAll.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsbSaveAll.Name = "tsbSaveAll";
            this.tsbSaveAll.Size = new System.Drawing.Size(115, 22);
            this.tsbSaveAll.Text = "Save All Textures";
            // 
            // TexturePreviewView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.tsContainer);
            this.Name = "TexturePreviewView";
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
        private System.Windows.Forms.ToolStrip tsToolbar;
        private System.Windows.Forms.ToolStripButton tsbSave;
        private System.Windows.Forms.ToolStripButton tsbSaveAll;
        private TextureView textureView;
    }
}
