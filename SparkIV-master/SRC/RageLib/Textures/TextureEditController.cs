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

using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;

namespace RageLib.Textures
{
    public class TextureEditController
    {
        private readonly TextureEditView _view;
        private readonly TextureViewController _textureViewController;
        private string _workingDirectory;

        public TextureEditController(TextureEditView view)
        {
            _view = view;
            _view.ExportClicked += View_ExportClicked;
            _view.ImportClicked += View_ImportClicked;

            _textureViewController = new TextureViewController(view.TextureView);
        }

        public event EventHandler SaveAndClose
        {
            add { _view.SaveCloseClicked += value; }
            remove { _view.SaveCloseClicked -= value; }
        }

        public TextureFile TextureFile
        {
            get { return _textureViewController.TextureFile; }
            set
            {
                _textureViewController.TextureFile = value;
                _view.TextureCount = value == null ? 0 : value.Count;
            }
        }

        private void View_ExportClicked(object sender, EventArgs e)
        {
            var texture = _view.TextureView.SelectedTexture;
            if (texture != null)
            {
                var sfd = new SaveFileDialog
                {
                    AddExtension = true,
                    OverwritePrompt = true,
                    Title = "Export Texture",
                    Filter = "Portable Network Graphics (*.png)|*.png",
                    InitialDirectory = _workingDirectory,
                    FileName = texture.TitleName + ".png"
                };

                if (sfd.ShowDialog() == DialogResult.OK)
                {
                    var image = texture.Decode();

                    var format = ImageFormat.Png;

                    image.Save(sfd.FileName, format);

                    _workingDirectory = new FileInfo(sfd.FileName).Directory.FullName;
                }
            }
        }

        private void View_ImportClicked(object sender, EventArgs e)
        {
            var texture = _view.TextureView.SelectedTexture;
            if (texture != null)
            {
                var ofd = new OpenFileDialog()
                {
                    AddExtension = true,
                    Title = "Import Texture",
                    Filter = "Portable Network Graphics (*.png)|*.png",
                    InitialDirectory = _workingDirectory,
                    FileName = texture.TitleName + ".png"
                };

                if (ofd.ShowDialog() == DialogResult.OK)
                {
                    var image = Image.FromFile(ofd.FileName);

                    texture.Encode(image);
                    
                    _workingDirectory = new FileInfo(ofd.FileName).Directory.FullName;

                    _textureViewController.UpdateImage();
                    _view.TextureView.RedrawTextureList();
                }
            }
        }



    }
}
