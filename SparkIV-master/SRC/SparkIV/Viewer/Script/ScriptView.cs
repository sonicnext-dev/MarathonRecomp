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

using System;
using System.Text;
using System.Windows.Forms;
using System.IO;
using RageLib.Scripting;

namespace SparkIV.Viewer.Script
{
    public partial class ScriptView : UserControl
    {
        ScriptFile _scriptFile;
        WebBrowser _browser;

        public ScriptView()
        {
            InitializeComponent();
        }

        internal ScriptFile ScriptFile
        {
            set
            {
                _scriptFile = value;
                UpdateView();
            }
        }

        void UpdateView()
        {
            CodeFormat format = CodeFormat.Disassemble;

            if (radioCFDecompile.Checked)
            {
                format = CodeFormat.ScruffDecompile;
            }
            else if (radioFullDecompile.Checked)
            {
                format = CodeFormat.FullDecompile;
            }

            string code = _scriptFile.GetCode(format);

            if (_browser != null)
            {
                Controls.Remove(_browser);
            }

            _browser = new WebBrowser();
            _browser.Dock = DockStyle.Fill;
            _browser.AllowNavigation = false;
            _browser.AllowWebBrowserDrop = false;
            //_browser.WebBrowserShortcutsEnabled = false;
            _browser.IsWebBrowserContextMenuEnabled = false;

            if (radioFullDecompile.Checked & code.Length < 0x10000)
            {
                var codeFormat = new ScruffFormat();
                string text = codeFormat.FormatCode(new MemoryStream(Encoding.ASCII.GetBytes(code)));
                text = "<style>" +
                            ".csharpcode, .csharpcode pre" + 
                            "{" + 
                            "  font-size: 10pt;" + 
                            "  color: black;" + 
                            "  font-family: Consolas, Courier New, Courier, Monospace;" + 
                            "  background-color: #ffffff;" + 
                            "}" + 
                            ".csharpcode pre { margin: 0em; }" + 
                            ".csharpcode .rem { color: #008000; }" + 
                            ".csharpcode .kwrd { color: #0000ff; }" + 
                            ".csharpcode .str { color: #006080; }" + 
                            ".csharpcode .op { color: #0000c0; }" + 
                            ".csharpcode .preproc { color: #cc6633; }" + 
                            ".csharpcode .asp { background-color: #ffff00; }" + 
                            ".csharpcode .html { color: #800000; }" + 
                            ".csharpcode .attr { color: #ff0000; }" + 
                            ".csharpcode .alt " + 
                            "{" + 
                            "  background-color: #f4f4f4;" + 
                            "  width: 100%;" + 
                            "  margin: 0em;" + 
                            "}" + 
                            ".csharpcode .lnum { color: #606060; }" + 
                        "</style>" +
                        text;
                _browser.DocumentText = text;
            }
            else
            {
                string text = "<pre>" + code.Replace("\n", "<br/>") + "</pre>";
                _browser.DocumentText = text;
            }

            Controls.Add(_browser);
            _browser.BringToFront();
        }

        private void radioCFDecompile_CheckedChanged(object sender, EventArgs e)
        {
            if (radioCFDecompile.Checked)
            {
                UpdateView();
            }
        }

        private void radioFullDecompile_CheckedChanged(object sender, EventArgs e)
        {
            if (radioFullDecompile.Checked)
            {
                UpdateView();
            }
        }

    }
}
