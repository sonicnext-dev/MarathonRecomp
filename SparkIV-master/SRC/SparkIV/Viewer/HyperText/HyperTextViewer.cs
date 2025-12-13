/**********************************************************************\

 Spark IV
 Copyright (C) 2009  Arushan/Aru <oneforaru at gmail.com>

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

using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;
using RageLib.HyperText;
using File=RageLib.FileSystem.Common.File;

namespace SparkIV.Viewer.HyperText
{
    public class HyperTextViewer : IViewer
    {
        public Control GetView(File file)
        {
            var data = file.GetData();

            var ms = new MemoryStream(data);
            var hyperTextFile = new HyperTextFile();
            try
            {
                hyperTextFile.Open(ms);
            }
            finally
            {
                ms.Close();
            }

            StringWriter sw = new StringWriter();
            hyperTextFile.WriteHTML(sw);

            // Create a temporary folder
            string tempPath = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());
            string htmlPath = Path.Combine(tempPath, "exported.html");

            Directory.CreateDirectory(tempPath);
            System.IO.File.WriteAllText(htmlPath, sw.ToString());

            if (hyperTextFile.EmbeddedTextureFile != null)
            {
                foreach (var texture in hyperTextFile.EmbeddedTextureFile)
                {
                    string imagePath = Path.Combine(tempPath, texture.Name + ".png");

                    string directory = Path.GetDirectoryName(imagePath);
                    if (!Directory.Exists(directory))
                    {
                        Directory.CreateDirectory(directory);
                    }

                    Image image = texture.Decode();
                    image.Save(imagePath, ImageFormat.Png);
                }
            }

            WebBrowser browser = new WebBrowser();
            browser.AllowNavigation = false;
            browser.AllowWebBrowserDrop = false;
            //_browser.WebBrowserShortcutsEnabled = false;
            //_browser.IsWebBrowserContextMenuEnabled = false;

            //browser.DocumentText = sw.ToString();
            browser.Navigate(htmlPath);

            browser.Disposed += delegate
                                    {
                                        Directory.Delete(tempPath, true);

                                        if (hyperTextFile.EmbeddedTextureFile != null)
                                        {
                                            hyperTextFile.EmbeddedTextureFile.Dispose();
                                        }
                                    };

            return browser;
        }
    }
}
