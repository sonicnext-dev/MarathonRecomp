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

using System.Reflection;
using System.Windows.Forms;
using System.Text;
using System.IO;
using System.Xml;
using System.Xml.Xsl;

namespace SparkIV.Viewer.Xml
{
    class XmlViewer : IViewer
    {
        public Control GetView(RageLib.FileSystem.Common.File file)
        {
            var data = file.GetData();

            Stream s = Assembly.GetExecutingAssembly().GetManifestResourceStream("SparkIV.Viewer.Xml.defaultss.xslt");

            XmlDocument doc = new XmlDocument();
            doc.InnerXml = Encoding.ASCII.GetString(data);

            XmlReader xr = XmlReader.Create(s);
            XslCompiledTransform xct = new XslCompiledTransform();
            xct.Load(xr);

            StringBuilder sb = new StringBuilder();
            XmlWriter xw = XmlWriter.Create(sb);
            xct.Transform(doc, xw);

            WebBrowser browser = new WebBrowser();
            browser.AllowNavigation = false;
            browser.AllowWebBrowserDrop = false;
            //browser.WebBrowserShortcutsEnabled = false;
            browser.IsWebBrowserContextMenuEnabled = false;

            browser.DocumentText = sb.ToString();
            return browser;
        }
    }
}
