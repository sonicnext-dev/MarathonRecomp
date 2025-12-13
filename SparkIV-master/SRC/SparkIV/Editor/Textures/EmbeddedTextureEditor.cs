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

using System.IO;
using RageLib.Common.Resources;
using RageLib.Textures;
using File=RageLib.FileSystem.Common.File;

namespace SparkIV.Editor.Textures
{
    class EmbeddedTextureEditor : TextureEditor
    {
        protected override void SaveAndClose(EditorForm form, TextureFile textureFile, File file)
        {
            using (new WaitCursor(form))
            {
                var resourceFile = new ResourceFile();
                using (var ms = new MemoryStream(file.GetData()))
                {
                    resourceFile.Read(ms);
                }

                var msSystem = new MemoryStream(resourceFile.SystemMemData);
                var msGraphics = new MemoryStream(resourceFile.GraphicsMemData);

                try
                {
                    textureFile.Save(msSystem, msGraphics);
                }
                finally
                {
                    msSystem.Close();
                    msGraphics.Close();
                }

                using (var resMS = new MemoryStream())
                {
                    resourceFile.Write(resMS);
                    file.SetData(resMS.ToArray());
                }
            }

            form.Close();

        }
    }
}
