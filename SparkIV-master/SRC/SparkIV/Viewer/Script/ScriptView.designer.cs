namespace SparkIV.Viewer.Script
{
    partial class ScriptView
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
            this.panel1 = new System.Windows.Forms.Panel();
            this.radioFullDecompile = new System.Windows.Forms.RadioButton();
            this.radioCFDecompile = new System.Windows.Forms.RadioButton();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.radioFullDecompile);
            this.panel1.Controls.Add(this.radioCFDecompile);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(443, 30);
            this.panel1.TabIndex = 0;
            // 
            // radioFullDecompile
            // 
            this.radioFullDecompile.Appearance = System.Windows.Forms.Appearance.Button;
            this.radioFullDecompile.AutoSize = true;
            this.radioFullDecompile.Checked = true;
            this.radioFullDecompile.Location = new System.Drawing.Point(3, 4);
            this.radioFullDecompile.Name = "radioFullDecompile";
            this.radioFullDecompile.Size = new System.Drawing.Size(127, 23);
            this.radioFullDecompile.TabIndex = 0;
            this.radioFullDecompile.TabStop = true;
            this.radioFullDecompile.Text = "High Level Decompiled";
            this.radioFullDecompile.UseVisualStyleBackColor = true;
            this.radioFullDecompile.CheckedChanged += new System.EventHandler(this.radioFullDecompile_CheckedChanged);
            // 
            // radioCFDecompile
            // 
            this.radioCFDecompile.Appearance = System.Windows.Forms.Appearance.Button;
            this.radioCFDecompile.AutoSize = true;
            this.radioCFDecompile.Location = new System.Drawing.Point(136, 4);
            this.radioCFDecompile.Name = "radioCFDecompile";
            this.radioCFDecompile.Size = new System.Drawing.Size(113, 23);
            this.radioCFDecompile.TabIndex = 1;
            this.radioCFDecompile.Text = "Scruff Disassembled";
            this.radioCFDecompile.UseVisualStyleBackColor = true;
            this.radioCFDecompile.CheckedChanged += new System.EventHandler(this.radioCFDecompile_CheckedChanged);
            // 
            // ScriptView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.panel1);
            this.Name = "ScriptView";
            this.Size = new System.Drawing.Size(443, 431);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.RadioButton radioFullDecompile;
        private System.Windows.Forms.RadioButton radioCFDecompile;
    }
}
