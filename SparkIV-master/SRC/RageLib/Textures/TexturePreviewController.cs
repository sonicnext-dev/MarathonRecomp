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
using System.Drawing.Imaging;
using System.IO;
using System.Windows.Forms;

namespace RageLib.Textures
{
    public class TexturePreviewController
    {
        private readonly TexturePreviewView _view;
        private readonly TextureViewController _textureViewController;
        private string _lastSaveDirectory;
        
        public TexturePreviewController(TexturePreviewView view)
        {
            _view = view;
            _view.SaveClicked += View_SaveClicked;
            _view.SaveAllClicked += View_SaveAllClicked;
            _view.Disposed += View_Disposed;

            _textureViewController = new TextureViewController(view.TextureView);
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

        private void View_SaveClicked(object sender, EventArgs e)
        {
            var texture = _view.TextureView.SelectedTexture;
            if (texture != null)
            {
                var sfd = new SaveFileDialog
                {
                    AddExtension = true,
                    OverwritePrompt = true,
                    Title = "Save Texture",
                    Filter = "Portable Network Graphics (*.png)|*.png|JPEG (*.jpg;*.jpeg)|*.jpg;*.jpeg",
                    InitialDirectory = _lastSaveDirectory,
                    FileName = texture.TitleName + ".png"
                };

                if (sfd.ShowDialog() == DialogResult.OK)
                {
                    var image = texture.Decode();

                    var format = ImageFormat.Png;
                    if (sfd.FileName.EndsWith(".jpg") || sfd.FileName.EndsWith(".jpeg"))
                    {
                        format = ImageFormat.Jpeg;
                    }

                    image.Save(sfd.FileName, format);

                    _lastSaveDirectory = new FileInfo(sfd.FileName).Directory.FullName;

                    MessageBox.Show("Texture saved.", "Save Texture", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
        }

        private void View_SaveAllClicked(object sender, EventArgs e)
        {
            var fbd = new FolderBrowserDialog
            {
                Description = "Select path to save textures to...",
                SelectedPath = _lastSaveDirectory,
                ShowNewFolderButton = true
            };

            if (fbd.ShowDialog() == DialogResult.OK)
            {
                foreach (var texture in _textureViewController.TextureFile)
                {
                    var image = texture.Decode();
                    image.Save(Path.Combine(fbd.SelectedPath, texture.TitleName + ".png"), ImageFormat.Png);
                }

                MessageBox.Show("Textures saved.", "Save All Textures", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        private void View_Disposed(object sender, EventArgs e)
        {
            // We handle this here instead of in the caller because the caller
            // doesn't know anything about the involved files...
            if (TextureFile != null)
            {
                TextureFile.Dispose();
                TextureFile = null;
            }
        }

    }
}
