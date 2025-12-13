/**********************************************************************\

 Resoursaur -- A Rage Resource File Decompressor
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

namespace Resoursaur
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
            this.lblType = new System.Windows.Forms.Label();
            this.lblTypeData = new System.Windows.Forms.Label();
            this.lblCompression = new System.Windows.Forms.Label();
            this.lblCompressionData = new System.Windows.Forms.Label();
            this.grpSysMem = new System.Windows.Forms.GroupBox();
            this.btnSysMemImport = new System.Windows.Forms.Button();
            this.btnSysMemExport = new System.Windows.Forms.Button();
            this.lblSysMemSize = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.lblGfxMemSize = new System.Windows.Forms.Label();
            this.btnGfxMemImport = new System.Windows.Forms.Button();
            this.btnGfxMemExport = new System.Windows.Forms.Button();
            this.btnSaveResource = new System.Windows.Forms.Button();
            this.btnOpenResource = new System.Windows.Forms.Button();
            this.lblFile = new System.Windows.Forms.Label();
            this.lblFileData = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.lnkWebsite = new System.Windows.Forms.LinkLabel();
            this.grpSysMem.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblType
            // 
            this.lblType.AutoSize = true;
            this.lblType.Location = new System.Drawing.Point(12, 37);
            this.lblType.Name = "lblType";
            this.lblType.Size = new System.Drawing.Size(34, 13);
            this.lblType.TabIndex = 4;
            this.lblType.Text = "Type:";
            // 
            // lblTypeData
            // 
            this.lblTypeData.AutoSize = true;
            this.lblTypeData.Location = new System.Drawing.Point(88, 37);
            this.lblTypeData.Name = "lblTypeData";
            this.lblTypeData.Size = new System.Drawing.Size(0, 13);
            this.lblTypeData.TabIndex = 5;
            // 
            // lblCompression
            // 
            this.lblCompression.AutoSize = true;
            this.lblCompression.Location = new System.Drawing.Point(12, 61);
            this.lblCompression.Name = "lblCompression";
            this.lblCompression.Size = new System.Drawing.Size(70, 13);
            this.lblCompression.TabIndex = 6;
            this.lblCompression.Text = "Compression:";
            // 
            // lblCompressionData
            // 
            this.lblCompressionData.AutoSize = true;
            this.lblCompressionData.Location = new System.Drawing.Point(88, 61);
            this.lblCompressionData.Name = "lblCompressionData";
            this.lblCompressionData.Size = new System.Drawing.Size(0, 13);
            this.lblCompressionData.TabIndex = 7;
            // 
            // grpSysMem
            // 
            this.grpSysMem.Controls.Add(this.lblSysMemSize);
            this.grpSysMem.Controls.Add(this.btnSysMemImport);
            this.grpSysMem.Controls.Add(this.btnSysMemExport);
            this.grpSysMem.Location = new System.Drawing.Point(15, 82);
            this.grpSysMem.Name = "grpSysMem";
            this.grpSysMem.Size = new System.Drawing.Size(240, 67);
            this.grpSysMem.TabIndex = 8;
            this.grpSysMem.TabStop = false;
            this.grpSysMem.Text = "System Memory Segment";
            // 
            // btnSysMemImport
            // 
            this.btnSysMemImport.Enabled = false;
            this.btnSysMemImport.Location = new System.Drawing.Point(92, 38);
            this.btnSysMemImport.Name = "btnSysMemImport";
            this.btnSysMemImport.Size = new System.Drawing.Size(68, 23);
            this.btnSysMemImport.TabIndex = 1;
            this.btnSysMemImport.Text = "Import";
            this.btnSysMemImport.UseVisualStyleBackColor = true;
            this.btnSysMemImport.Click += new System.EventHandler(this.btnSysMemImport_Click);
            // 
            // btnSysMemExport
            // 
            this.btnSysMemExport.Enabled = false;
            this.btnSysMemExport.Location = new System.Drawing.Point(166, 38);
            this.btnSysMemExport.Name = "btnSysMemExport";
            this.btnSysMemExport.Size = new System.Drawing.Size(68, 23);
            this.btnSysMemExport.TabIndex = 2;
            this.btnSysMemExport.Text = "Export";
            this.btnSysMemExport.UseVisualStyleBackColor = true;
            this.btnSysMemExport.Click += new System.EventHandler(this.btnSysMemExport_Click);
            // 
            // lblSysMemSize
            // 
            this.lblSysMemSize.AutoSize = true;
            this.lblSysMemSize.Location = new System.Drawing.Point(7, 20);
            this.lblSysMemSize.Name = "lblSysMemSize";
            this.lblSysMemSize.Size = new System.Drawing.Size(0, 13);
            this.lblSysMemSize.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.lblGfxMemSize);
            this.groupBox2.Controls.Add(this.btnGfxMemImport);
            this.groupBox2.Controls.Add(this.btnGfxMemExport);
            this.groupBox2.Location = new System.Drawing.Point(15, 155);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(240, 67);
            this.groupBox2.TabIndex = 9;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Graphics Memory Segment";
            // 
            // lblGfxMemSize
            // 
            this.lblGfxMemSize.AutoSize = true;
            this.lblGfxMemSize.Location = new System.Drawing.Point(7, 20);
            this.lblGfxMemSize.Name = "lblGfxMemSize";
            this.lblGfxMemSize.Size = new System.Drawing.Size(0, 13);
            this.lblGfxMemSize.TabIndex = 0;
            // 
            // btnGfxMemImport
            // 
            this.btnGfxMemImport.Enabled = false;
            this.btnGfxMemImport.Location = new System.Drawing.Point(92, 38);
            this.btnGfxMemImport.Name = "btnGfxMemImport";
            this.btnGfxMemImport.Size = new System.Drawing.Size(68, 23);
            this.btnGfxMemImport.TabIndex = 1;
            this.btnGfxMemImport.Text = "Import";
            this.btnGfxMemImport.UseVisualStyleBackColor = true;
            this.btnGfxMemImport.Click += new System.EventHandler(this.btnGfxMemImport_Click);
            // 
            // btnGfxMemExport
            // 
            this.btnGfxMemExport.Enabled = false;
            this.btnGfxMemExport.Location = new System.Drawing.Point(166, 38);
            this.btnGfxMemExport.Name = "btnGfxMemExport";
            this.btnGfxMemExport.Size = new System.Drawing.Size(68, 23);
            this.btnGfxMemExport.TabIndex = 2;
            this.btnGfxMemExport.Text = "Export";
            this.btnGfxMemExport.UseVisualStyleBackColor = true;
            this.btnGfxMemExport.Click += new System.EventHandler(this.btnGfxMemExport_Click);
            // 
            // btnSaveResource
            // 
            this.btnSaveResource.Enabled = false;
            this.btnSaveResource.Location = new System.Drawing.Point(138, 236);
            this.btnSaveResource.Name = "btnSaveResource";
            this.btnSaveResource.Size = new System.Drawing.Size(117, 23);
            this.btnSaveResource.TabIndex = 1;
            this.btnSaveResource.Text = "Save Resource";
            this.btnSaveResource.UseVisualStyleBackColor = true;
            this.btnSaveResource.Click += new System.EventHandler(this.btnSaveResource_Click);
            // 
            // btnOpenResource
            // 
            this.btnOpenResource.Location = new System.Drawing.Point(15, 236);
            this.btnOpenResource.Name = "btnOpenResource";
            this.btnOpenResource.Size = new System.Drawing.Size(117, 23);
            this.btnOpenResource.TabIndex = 0;
            this.btnOpenResource.Text = "Open Resource";
            this.btnOpenResource.UseVisualStyleBackColor = true;
            this.btnOpenResource.Click += new System.EventHandler(this.btnOpenResource_Click);
            // 
            // lblFile
            // 
            this.lblFile.AutoSize = true;
            this.lblFile.Location = new System.Drawing.Point(12, 14);
            this.lblFile.Name = "lblFile";
            this.lblFile.Size = new System.Drawing.Size(26, 13);
            this.lblFile.TabIndex = 2;
            this.lblFile.Text = "File:";
            // 
            // lblFileData
            // 
            this.lblFileData.AutoSize = true;
            this.lblFileData.Location = new System.Drawing.Point(88, 14);
            this.lblFileData.Name = "lblFileData";
            this.lblFileData.Size = new System.Drawing.Size(0, 13);
            this.lblFileData.TabIndex = 3;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Arial", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label9.Location = new System.Drawing.Point(13, 270);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(118, 12);
            this.label9.TabIndex = 10;
            this.label9.Text = "Resoursaur - (C) 2008, Aru";
            // 
            // lnkWebsite
            // 
            this.lnkWebsite.AutoSize = true;
            this.lnkWebsite.Font = new System.Drawing.Font("Arial", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lnkWebsite.Location = new System.Drawing.Point(173, 270);
            this.lnkWebsite.Name = "lnkWebsite";
            this.lnkWebsite.Size = new System.Drawing.Size(82, 12);
            this.lnkWebsite.TabIndex = 11;
            this.lnkWebsite.TabStop = true;
            this.lnkWebsite.Text = "www.quackler.com";
            this.lnkWebsite.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.lnkWebsite_LinkClicked);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(267, 295);
            this.Controls.Add(this.lnkWebsite);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.lblFileData);
            this.Controls.Add(this.lblFile);
            this.Controls.Add(this.btnSaveResource);
            this.Controls.Add(this.btnOpenResource);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.grpSysMem);
            this.Controls.Add(this.lblCompressionData);
            this.Controls.Add(this.lblCompression);
            this.Controls.Add(this.lblTypeData);
            this.Controls.Add(this.lblType);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Resoursaur - GTAIV Resource Tool";
            this.grpSysMem.ResumeLayout(false);
            this.grpSysMem.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lblType;
        private System.Windows.Forms.Label lblTypeData;
        private System.Windows.Forms.Label lblCompression;
        private System.Windows.Forms.Label lblCompressionData;
        private System.Windows.Forms.GroupBox grpSysMem;
        private System.Windows.Forms.Button btnSysMemImport;
        private System.Windows.Forms.Button btnSysMemExport;
        private System.Windows.Forms.Label lblSysMemSize;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label lblGfxMemSize;
        private System.Windows.Forms.Button btnGfxMemImport;
        private System.Windows.Forms.Button btnGfxMemExport;
        private System.Windows.Forms.Button btnSaveResource;
        private System.Windows.Forms.Button btnOpenResource;
        private System.Windows.Forms.Label lblFile;
        private System.Windows.Forms.Label lblFileData;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.LinkLabel lnkWebsite;
    }
}

