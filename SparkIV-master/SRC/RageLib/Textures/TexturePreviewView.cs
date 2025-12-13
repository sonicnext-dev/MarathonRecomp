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
using System.Windows.Forms;

namespace RageLib.Textures
{
    public partial class TexturePreviewView : UserControl
    {
        public TexturePreviewView()
        {
            InitializeComponent();
        }

        public int TextureCount
        {
            set
            {
                tslTexturesInfo.Text = value + " Texture" + (value == 1 ? "" : "s");
            }
        }

        public TextureView TextureView
        {
            get
            {
                return textureView;
            }
        }

        public event EventHandler SaveClicked
        {
            add { tsbSave.Click += value; }
            remove { tsbSave.Click -= value; }
        }

        public event EventHandler SaveAllClicked
        {
            add { tsbSaveAll.Click += value; }
            remove { tsbSaveAll.Click -= value; }
        }
    }
}
