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
    partial class TextureView
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TextureView));
            this.panelPreview = new System.Windows.Forms.Panel();
            this.picPreview = new System.Windows.Forms.PictureBox();
            this.splitContainer = new System.Windows.Forms.SplitContainer();
            this.listTextures = new System.Windows.Forms.ListBox();
            this.panelInfo = new System.Windows.Forms.Panel();
            this.panelInfoControls = new System.Windows.Forms.Panel();
            this.lblMipMap = new System.Windows.Forms.Label();
            this.cboMipMaps = new System.Windows.Forms.ComboBox();
            this.radioChannelA = new System.Windows.Forms.RadioButton();
            this.radioChannelB = new System.Windows.Forms.RadioButton();
            this.radioChannelG = new System.Windows.Forms.RadioButton();
            this.radioChannelR = new System.Windows.Forms.RadioButton();
            this.radioChannelFull = new System.Windows.Forms.RadioButton();
            this.lblTextureFormat = new System.Windows.Forms.Label();
            this.lblTextureDims = new System.Windows.Forms.Label();
            this.lblTextureName = new System.Windows.Forms.Label();
            this.panelPreview.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picPreview)).BeginInit();
            this.splitContainer.Panel1.SuspendLayout();
            this.splitContainer.Panel2.SuspendLayout();
            this.splitContainer.SuspendLayout();
            this.panelInfo.SuspendLayout();
            this.panelInfoControls.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelPreview
            // 
            this.panelPreview.AutoScroll = true;
            this.panelPreview.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.panelPreview.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("panelPreview.BackgroundImage")));
            this.panelPreview.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panelPreview.Controls.Add(this.picPreview);
            this.panelPreview.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelPreview.Location = new System.Drawing.Point(0, 0);
            this.panelPreview.Name = "panelPreview";
            this.panelPreview.Size = new System.Drawing.Size(459, 431);
            this.panelPreview.TabIndex = 1;
            // 
            // picPreview
            // 
            this.picPreview.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.picPreview.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("picPreview.BackgroundImage")));
            this.picPreview.Location = new System.Drawing.Point(0, 0);
            this.picPreview.Name = "picPreview";
            this.picPreview.Size = new System.Drawing.Size(1, 1);
            this.picPreview.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.picPreview.TabIndex = 0;
            this.picPreview.TabStop = false;
            // 
            // splitContainer
            // 
            this.splitContainer.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer.Location = new System.Drawing.Point(0, 0);
            this.splitContainer.Name = "splitContainer";
            // 
            // splitContainer.Panel1
            // 
            this.splitContainer.Panel1.Controls.Add(this.listTextures);
            // 
            // splitContainer.Panel2
            // 
            this.splitContainer.Panel2.Controls.Add(this.panelPreview);
            this.splitContainer.Panel2.Controls.Add(this.panelInfo);
            this.splitContainer.Size = new System.Drawing.Size(633, 487);
            this.splitContainer.SplitterDistance = 170;
            this.splitContainer.TabIndex = 2;
            // 
            // listTextures
            // 
            this.listTextures.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listTextures.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawVariable;
            this.listTextures.FormattingEnabled = true;
            this.listTextures.Location = new System.Drawing.Point(0, 0);
            this.listTextures.Name = "listTextures";
            this.listTextures.Size = new System.Drawing.Size(170, 487);
            this.listTextures.TabIndex = 0;
            this.listTextures.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.listTextures_DrawItem);
            this.listTextures.MeasureItem += new System.Windows.Forms.MeasureItemEventHandler(this.listTextures_MeasureItem);
            // 
            // panelInfo
            // 
            this.panelInfo.Controls.Add(this.panelInfoControls);
            this.panelInfo.Controls.Add(this.lblTextureFormat);
            this.panelInfo.Controls.Add(this.lblTextureDims);
            this.panelInfo.Controls.Add(this.lblTextureName);
            this.panelInfo.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panelInfo.Enabled = false;
            this.panelInfo.Location = new System.Drawing.Point(0, 431);
            this.panelInfo.Name = "panelInfo";
            this.panelInfo.Size = new System.Drawing.Size(459, 56);
            this.panelInfo.TabIndex = 2;
            // 
            // panelInfoControls
            // 
            this.panelInfoControls.Controls.Add(this.lblMipMap);
            this.panelInfoControls.Controls.Add(this.cboMipMaps);
            this.panelInfoControls.Controls.Add(this.radioChannelA);
            this.panelInfoControls.Controls.Add(this.radioChannelB);
            this.panelInfoControls.Controls.Add(this.radioChannelG);
            this.panelInfoControls.Controls.Add(this.radioChannelR);
            this.panelInfoControls.Controls.Add(this.radioChannelFull);
            this.panelInfoControls.Dock = System.Windows.Forms.DockStyle.Right;
            this.panelInfoControls.Location = new System.Drawing.Point(309, 0);
            this.panelInfoControls.Name = "panelInfoControls";
            this.panelInfoControls.Size = new System.Drawing.Size(150, 56);
            this.panelInfoControls.TabIndex = 4;
            // 
            // lblMipMap
            // 
            this.lblMipMap.AutoSize = true;
            this.lblMipMap.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblMipMap.Location = new System.Drawing.Point(3, 33);
            this.lblMipMap.Name = "lblMipMap";
            this.lblMipMap.Size = new System.Drawing.Size(48, 13);
            this.lblMipMap.TabIndex = 6;
            this.lblMipMap.Text = "MipMap:";
            // 
            // cboMipMaps
            // 
            this.cboMipMaps.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboMipMaps.FormattingEnabled = true;
            this.cboMipMaps.Location = new System.Drawing.Point(57, 30);
            this.cboMipMaps.Name = "cboMipMaps";
            this.cboMipMaps.Size = new System.Drawing.Size(89, 21);
            this.cboMipMaps.TabIndex = 5;
            // 
            // radioChannelA
            // 
            this.radioChannelA.Appearance = System.Windows.Forms.Appearance.Button;
            this.radioChannelA.AutoSize = true;
            this.radioChannelA.Location = new System.Drawing.Point(122, 3);
            this.radioChannelA.Name = "radioChannelA";
            this.radioChannelA.Size = new System.Drawing.Size(24, 23);
            this.radioChannelA.TabIndex = 4;
            this.radioChannelA.Text = "A";
            this.radioChannelA.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.radioChannelA.UseVisualStyleBackColor = true;
            this.radioChannelA.CheckedChanged += new System.EventHandler(this.view_ImageChannelChecked);
            // 
            // radioChannelB
            // 
            this.radioChannelB.Appearance = System.Windows.Forms.Appearance.Button;
            this.radioChannelB.AutoSize = true;
            this.radioChannelB.Location = new System.Drawing.Point(96, 3);
            this.radioChannelB.Name = "radioChannelB";
            this.radioChannelB.Size = new System.Drawing.Size(24, 23);
            this.radioChannelB.TabIndex = 3;
            this.radioChannelB.Text = "B";
            this.radioChannelB.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.radioChannelB.UseVisualStyleBackColor = true;
            this.radioChannelB.CheckedChanged += new System.EventHandler(this.view_ImageChannelChecked);
            // 
            // radioChannelG
            // 
            this.radioChannelG.Appearance = System.Windows.Forms.Appearance.Button;
            this.radioChannelG.AutoSize = true;
            this.radioChannelG.Location = new System.Drawing.Point(69, 3);
            this.radioChannelG.Name = "radioChannelG";
            this.radioChannelG.Size = new System.Drawing.Size(25, 23);
            this.radioChannelG.TabIndex = 2;
            this.radioChannelG.Text = "G";
            this.radioChannelG.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.radioChannelG.UseVisualStyleBackColor = true;
            this.radioChannelG.CheckedChanged += new System.EventHandler(this.view_ImageChannelChecked);
            // 
            // radioChannelR
            // 
            this.radioChannelR.Appearance = System.Windows.Forms.Appearance.Button;
            this.radioChannelR.AutoSize = true;
            this.radioChannelR.Location = new System.Drawing.Point(42, 3);
            this.radioChannelR.Name = "radioChannelR";
            this.radioChannelR.Size = new System.Drawing.Size(25, 23);
            this.radioChannelR.TabIndex = 1;
            this.radioChannelR.Text = "R";
            this.radioChannelR.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.radioChannelR.UseVisualStyleBackColor = true;
            this.radioChannelR.CheckedChanged += new System.EventHandler(this.view_ImageChannelChecked);
            // 
            // radioChannelFull
            // 
            this.radioChannelFull.Appearance = System.Windows.Forms.Appearance.Button;
            this.radioChannelFull.AutoSize = true;
            this.radioChannelFull.Checked = true;
            this.radioChannelFull.Location = new System.Drawing.Point(3, 3);
            this.radioChannelFull.Name = "radioChannelFull";
            this.radioChannelFull.Size = new System.Drawing.Size(33, 23);
            this.radioChannelFull.TabIndex = 0;
            this.radioChannelFull.TabStop = true;
            this.radioChannelFull.Text = "Full";
            this.radioChannelFull.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.radioChannelFull.UseVisualStyleBackColor = true;
            this.radioChannelFull.CheckedChanged += new System.EventHandler(this.view_ImageChannelChecked);
            // 
            // lblTextureFormat
            // 
            this.lblTextureFormat.AutoSize = true;
            this.lblTextureFormat.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblTextureFormat.Location = new System.Drawing.Point(4, 33);
            this.lblTextureFormat.Name = "lblTextureFormat";
            this.lblTextureFormat.Size = new System.Drawing.Size(0, 13);
            this.lblTextureFormat.TabIndex = 2;
            // 
            // lblTextureDims
            // 
            this.lblTextureDims.AutoSize = true;
            this.lblTextureDims.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblTextureDims.Location = new System.Drawing.Point(4, 20);
            this.lblTextureDims.Name = "lblTextureDims";
            this.lblTextureDims.Size = new System.Drawing.Size(0, 13);
            this.lblTextureDims.TabIndex = 1;
            // 
            // lblTextureName
            // 
            this.lblTextureName.AutoSize = true;
            this.lblTextureName.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblTextureName.Location = new System.Drawing.Point(4, 7);
            this.lblTextureName.Name = "lblTextureName";
            this.lblTextureName.Size = new System.Drawing.Size(0, 13);
            this.lblTextureName.TabIndex = 0;
            // 
            // TextureView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.splitContainer);
            this.Name = "TextureView";
            this.Size = new System.Drawing.Size(633, 487);
            this.panelPreview.ResumeLayout(false);
            this.panelPreview.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.picPreview)).EndInit();
            this.splitContainer.Panel1.ResumeLayout(false);
            this.splitContainer.Panel2.ResumeLayout(false);
            this.splitContainer.ResumeLayout(false);
            this.panelInfo.ResumeLayout(false);
            this.panelInfo.PerformLayout();
            this.panelInfoControls.ResumeLayout(false);
            this.panelInfoControls.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panelPreview;
        private System.Windows.Forms.PictureBox picPreview;
        private System.Windows.Forms.SplitContainer splitContainer;
        private System.Windows.Forms.ListBox listTextures;
        private System.Windows.Forms.Panel panelInfo;
        private System.Windows.Forms.Label lblTextureName;
        private System.Windows.Forms.Label lblTextureFormat;
        private System.Windows.Forms.Label lblTextureDims;
        private System.Windows.Forms.Panel panelInfoControls;
        private System.Windows.Forms.RadioButton radioChannelFull;
        private System.Windows.Forms.RadioButton radioChannelA;
        private System.Windows.Forms.RadioButton radioChannelB;
        private System.Windows.Forms.RadioButton radioChannelG;
        private System.Windows.Forms.RadioButton radioChannelR;
        private System.Windows.Forms.Label lblMipMap;
        private System.Windows.Forms.ComboBox cboMipMaps;
    }
}