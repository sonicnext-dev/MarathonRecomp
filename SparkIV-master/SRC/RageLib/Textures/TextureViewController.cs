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
using RageLib.Textures.Filter;

namespace RageLib.Textures
{
    class TextureViewController
    {
        private readonly TextureView _view;
        private TextureFile _textureFile;

        public TextureViewController(TextureView view)
        {
            _view = view;

            _view.SelectedTextureChanged += View_SelectedTextureChanged;
            _view.SelectedMipMapChanged += View_SelectedMipMapChanged;
            _view.ImageChannelChanged += View_ImageChannelChanged;
        }

        public TextureFile TextureFile
        {
            get { return _textureFile; }
            set
            {
                _textureFile = value;
                UpdateView();
            }
        }

        private void UpdateView()
        {
            _view.ClearTextures();
            
            if (_textureFile != null)
            {
                foreach (var texture in _textureFile)
                {
                    _view.AddTexture(texture);
                }
                if (_textureFile.Count > 0)
                {
                    _view.SelectedTexture = _textureFile.Textures[0];
                    _view.InfoPanelEnabled = true;
                }
            }
            else
            {
                _view.InfoPanelEnabled = false;
            }
        }

        private void View_SelectedTextureChanged(object sender, EventArgs e)
        {
            var texture = _view.SelectedTexture;
            if (texture != null)
            {
                _view.SetTextureInfo(texture.TitleName, texture.TextureType, texture.Width, texture.Height, texture.Levels);
                UpdateImage();
            }
        }

        private void View_SelectedMipMapChanged(object sender, EventArgs e)
        {
            UpdateImage();
        }


        private void View_ImageChannelChanged(object sender, EventArgs e)
        {
            UpdateImage();
        }

        public void UpdateImage()
        {
            var texture = _view.SelectedTexture;
            if (texture != null)
            {
                var image = texture.Decode(_view.SelectedMipMap);
                if (_view.SelectedImageChannel != ImageChannel.All)
                {
                    var channelFilter = new ChannelFilter(_view.SelectedImageChannel);
                    channelFilter.Apply(image);
                }
                _view.PreviewImage = image;
            }
        }
    }
}
