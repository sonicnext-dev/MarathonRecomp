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

using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using RageLib.Models;
using RageLib.Textures;
using File = RageLib.FileSystem.Common.File;

namespace SparkIV.Viewer.Models
{
    class ModelViewer : IViewer
    {
        protected Control CreateControl(File file, IModelFile modelfile)
        {
            var view = new ModelView();
            var controller = new ModelViewController(view);

            var fileName = file.Name;
            var fileNameWOE = fileName.Substring(0, fileName.LastIndexOf('.'));

            List<TextureFile> textures = new List<TextureFile>();
            TryLoadTexture(textures, file, fileNameWOE);
            TryLoadTexture(textures, file, "vehshare");
            TryLoadTexture(textures, file, "vehshare_truck");
            controller.TextureFiles = textures.ToArray();

            controller.ModelFile = modelfile;
            return view;
        }

        private void TryLoadTexture(List<TextureFile> textureList, File file, string name)
        {
            var textureFileName = name + ".wtd";
            var textures = file.ParentDirectory.FindByName(textureFileName) as File;
            if (textures != null)
            {
                var textureFile = new TextureFile();
                var textureMS = new MemoryStream(textures.GetData());
                try
                {
                    textureFile.Open(textureMS);
                    textureList.Add(textureFile);
                }
                finally
                {
                    textureMS.Close();
                }
            }
        }

        #region Implementation of IViewer

        public virtual Control GetView(File file)
        {
            var data = file.GetData();

            var ms = new MemoryStream(data);
            var modelFile = new ModelFile();
            try
            {
                modelFile.Open(ms);
            }
            finally
            {
                ms.Close();
            }

            return CreateControl(file, modelFile);
        }

        #endregion
    }
}
