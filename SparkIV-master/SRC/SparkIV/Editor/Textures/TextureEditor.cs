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
using RageLib.FileSystem.Common;
using RageLib.Textures;
using File=RageLib.FileSystem.Common.File;

namespace SparkIV.Editor.Textures
{
    class TextureEditor : IEditor
    {
        public virtual void LaunchEditor(FileSystem fs, File file)
        {
            var data = file.GetData();

            var ms = new MemoryStream(data);
            var textureFile = new TextureFile();
            try
            {
                textureFile.Open(ms);
            }
            finally
            {
                ms.Close();
            }

            ShowForm(file, textureFile);
        }

        protected void ShowForm(File file, TextureFile textureFile)
        {
            var view = new TextureEditView();
            
            var controller = new TextureEditController(view);
            controller.TextureFile = textureFile;

            using (var form = new EditorForm())
            {
                form.SetFilename(file.Name);
                form.SetControl(view);

                controller.SaveAndClose += ((sender, e) => SaveAndClose(form, textureFile, file));

                form.ShowDialog();
            }

            textureFile.Dispose();
        }

        protected virtual void SaveAndClose(EditorForm form, TextureFile textureFile, File file)
        {
            using (new WaitCursor(form))
            {
                var msSave = new MemoryStream();
                try
                {
                    textureFile.Save(msSave);

                    file.SetData(msSave.ToArray());
                }
                finally
                {
                    msSave.Close();
                }
            }

            form.Close();
        }
    }
}
