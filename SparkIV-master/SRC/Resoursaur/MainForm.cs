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

using System;
using System.Diagnostics;
using System.IO;
using System.Windows.Forms;
using RageLib.Common.Resources;

namespace Resoursaur
{
    public partial class MainForm : Form
    {
        private ResourceFile _resourceFile;
        private string _filename;

        public MainForm()
        {
            InitializeComponent();
        }

        private string OpenFileForRead(string title, string defaultFilename)
        {
            var ofd = new OpenFileDialog
                          {
                              Title = title, 
                              FileName = defaultFilename,
                              CheckFileExists = true,
                          };

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                return ofd.FileName;
            }
            else
            {
                return null;
            }
        }


        private string OpenFileForWrite(string title, string defaultFilename)
        {
            var ofd = new SaveFileDialog
            {
                Title = title,
                FileName = defaultFilename,
                OverwritePrompt = true,
            };

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                return ofd.FileName;
            }
            else
            {
                return null;
            }
        }


        private void UpdateUI()
        {
            var f = new FileInfo(_filename);
            lblFileData.Text = f.Name;

            string rscType = Enum.IsDefined(_resourceFile.Type.GetType(), _resourceFile.Type)
                                 ? _resourceFile.Type.ToString()
                                 : string.Format("Unknown 0x{0:x}", (int) _resourceFile.Type);
            lblTypeData.Text = rscType;

            lblCompressionData.Text = _resourceFile.Compression.ToString();

            lblSysMemSize.Text = string.Format("Size: {0} bytes", _resourceFile.SystemMemSize.ToString("N0"));
            lblGfxMemSize.Text = string.Format("Size: {0} bytes", _resourceFile.GraphicsMemSize.ToString("N0"));

            btnGfxMemExport.Enabled = true;
            btnSysMemExport.Enabled = true;
            btnGfxMemImport.Enabled = true;
            btnSysMemImport.Enabled = true;
            btnSaveResource.Enabled = true;
        }

        private void btnOpenResource_Click(object sender, EventArgs e)
        {
            var filename = OpenFileForRead("Open Resource...", null);
            if (filename != null)
            {
                var res = new ResourceFile();
                var fs = new FileStream(filename, FileMode.Open, FileAccess.Read);
                try
                {
                    res.Read(fs);

                    _resourceFile = res;
                    _filename = filename;

                    UpdateUI();
                }
                catch ( Exception exception )
                {
                    MessageBox.Show(exception.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                finally
                {
                    fs.Close();
                }
            }
        }

        private void btnSaveResource_Click(object sender, EventArgs e)
        {
            var fs = new FileStream(_filename, FileMode.Create, FileAccess.Write);
            try
            {
                _resourceFile.Write( fs );

                MessageBox.Show("The resource file has been saved.", "Save", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch (Exception exception)
            {
                MessageBox.Show(exception.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            finally
            {
                fs.Close();
            }

        }

        private void btnSysMemImport_Click(object sender, EventArgs e)
        {
            var filename = OpenFileForRead("Import System Memory Segment...", _filename + ".sys");
            if (filename != null)
            {
                var data = File.ReadAllBytes(filename);
                if ((data.Length % 0x1000) != 0)
                {
                    MessageBox.Show("The segment size must be a multiple of 4096 bytes.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    _resourceFile.SystemMemData = data;
                    UpdateUI();

                    MessageBox.Show("The segment has been imported.", "Import", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        private void btnGfxMemImport_Click(object sender, EventArgs e)
        {
            var filename = OpenFileForRead("Import Graphics Memory Segment...", _filename + ".gfx");
            if (filename != null)
            {
                var data = File.ReadAllBytes(filename);
                if ((data.Length % 0x1000) != 0)
                {
                    MessageBox.Show("The segment size must be a multiple of 4096 bytes.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    _resourceFile.GraphicsMemData = data;
                    UpdateUI();

                    MessageBox.Show("The segment has been imported.", "Import", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }

        }

        private void btnSysMemExport_Click(object sender, EventArgs e)
        {
            var filename = OpenFileForWrite("Export System Memory Segment...", _filename + ".sys");
            if (filename != null)
            {
                File.WriteAllBytes(filename, _resourceFile.SystemMemData);
                MessageBox.Show("The segment has been exported.", "Export", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void btnGfxMemExport_Click(object sender, EventArgs e)
        {
            var filename = OpenFileForWrite("Export Graphics Memory Segment...", _filename + ".gfx");
            if (filename != null)
            {
                File.WriteAllBytes(filename, _resourceFile.GraphicsMemData);
                MessageBox.Show("The segment has been exported.", "Export", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void lnkWebsite_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start("http://www.quackler.com");
        }
    }
}
