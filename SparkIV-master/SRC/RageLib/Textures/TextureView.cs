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
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using RageLib.Textures.Filter;
using Brush=System.Drawing.Brush;
using FontStyle=System.Drawing.FontStyle;
using Graphics=System.Drawing.Graphics;
using SystemBrushes=System.Drawing.SystemBrushes;

namespace RageLib.Textures
{
    public partial class TextureView : UserControl
    {
        private const int TextureListIconPadding = 2;
        private const int TextureListIconSize = Texture.ThumbnailSize;

        public event EventHandler ImageChannelChanged;

        public TextureView()
        {
            InitializeComponent();

            ClearTextures();
        }

        public event EventHandler SelectedTextureChanged
        {
            add { listTextures.SelectedIndexChanged += value; }
            remove { listTextures.SelectedIndexChanged -= value; }
        }

        public event EventHandler SelectedMipMapChanged
        {
            add { cboMipMaps.SelectedIndexChanged += value; }
            remove { cboMipMaps.SelectedIndexChanged -= value; }
        }

        public Texture SelectedTexture
        {
            get { return listTextures.SelectedItem as Texture; }
            set { listTextures.SelectedItem = value; }
        }

        public void ClearTextures()
        {
            listTextures.SelectedItem = null;
            listTextures.Items.Clear();
            picPreview.Image = null;
            picPreview.Size = new Size(1, 1);
        }

        public void AddTexture(Texture texture)
        {
            listTextures.Items.Add(texture);
        }

        public Image PreviewImage
        {
            get { return picPreview.Image; }
            set { picPreview.Image = value; }
        }

        public bool InfoPanelEnabled
        {
            get { return panelInfo.Enabled; }
            set { panelInfo.Enabled = value; }
        }

        internal void SetTextureInfo(string name, TextureType type, uint width, uint height, int levels)
        {
            lblTextureName.Text = name;
            lblTextureFormat.Text = type.ToString();
            lblTextureDims.Text = string.Format("{0}x{1}", width, height);

            cboMipMaps.Items.Clear();
            for(int i=0; i<levels; i++)
            {
                string levelText = string.Format("{0}x{1}", width, height);
                cboMipMaps.Items.Add(levelText);

                width /= 2;
                height /= 2;
            }

            cboMipMaps.SelectedIndex = 0;
        }

        public int SelectedMipMap
        {
            get { return cboMipMaps.SelectedIndex; }
        }

        internal ImageChannel SelectedImageChannel
        {
            get
            {
                var channel = ImageChannel.All;
                channel = (radioChannelR.Checked) ? ImageChannel.Red : channel;
                channel = (radioChannelG.Checked) ? ImageChannel.Green : channel;
                channel = (radioChannelB.Checked) ? ImageChannel.Blue : channel;
                channel = (radioChannelA.Checked) ? ImageChannel.Alpha : channel;
                return channel;
            }
            set
            {
                switch(value)
                {
                    case ImageChannel.All:
                        radioChannelFull.Checked = true;
                        break;
                    case ImageChannel.Red:
                        radioChannelR.Checked = true;
                        break;
                    case ImageChannel.Green:
                        radioChannelG.Checked = true;
                        break;
                    case ImageChannel.Blue:
                        radioChannelB.Checked = true;
                        break;
                    case ImageChannel.Alpha:
                        radioChannelA.Checked = true;
                        break;
                    default:
                        throw new ArgumentOutOfRangeException("value");
                }
            }
        }

        public void RedrawTextureList()
        {
            listTextures.Invalidate();
        }

        private void view_ImageChannelChecked(object sender, EventArgs e)
        {
            if (ImageChannelChanged != null)
            {
                ImageChannelChanged(this, null);
            }
        }

        private void listTextures_MeasureItem(object sender, MeasureItemEventArgs e)
        {
            e.ItemHeight = TextureListIconPadding * 2 + TextureListIconSize;
            e.ItemWidth = listTextures.Width;
        }

        private void listTextures_DrawItem(object sender, DrawItemEventArgs e)
        {
            if (e.Index < 0)
                return;

            bool selected = ((e.State & DrawItemState.Selected) != 0);

            var texture = listTextures.Items[e.Index] as Texture;
            if (texture == null)
                return;

            string format = texture.TextureType.ToString();
            string textMain = texture.TitleName;
            string textSub = texture.Width + "x" + texture.Height + " (" + format + ")";
            Font fontNormal = listTextures.Font;
            Font fontBold = new Font(fontNormal, FontStyle.Bold);
            Brush brushFG = selected ? SystemBrushes.HighlightText : SystemBrushes.ControlText;

            Graphics g = e.Graphics;

            // Clear the background
            if (selected)
            {
                Brush brushBG =
                    new LinearGradientBrush(e.Bounds, SystemColors.Highlight, SystemColors.HotTrack,
                                            LinearGradientMode.Horizontal);
                g.FillRectangle(brushBG, e.Bounds);
            }
            else
            {
                Brush brushBG = SystemBrushes.Window;
                g.FillRectangle(brushBG, e.Bounds);
            }

            Image thumbnail = texture.DecodeAsThumbnail();

            // Draw the icon
            int iconLeft = TextureListIconPadding + (TextureListIconSize - thumbnail.Width) / 2;
            int iconTop = TextureListIconPadding + (TextureListIconSize - thumbnail.Height) / 2;
            g.DrawImage(thumbnail, iconLeft, iconTop + e.Bounds.Top, thumbnail.Width, thumbnail.Height);

            // Draw the text
            int textLeft = TextureListIconSize + TextureListIconPadding * 2;

            SizeF sizeMain = g.MeasureString(textMain, fontBold);
            SizeF sizeSub = g.MeasureString(textSub, fontNormal);

            int textSpacer = (int)(e.Bounds.Height - (sizeMain.Height + sizeSub.Height + TextureListIconPadding * 2)) / 2;

            g.DrawString(textMain, fontBold, brushFG, textLeft, textSpacer + e.Bounds.Top + TextureListIconPadding);
            g.DrawString(textSub, fontNormal, brushFG, textLeft,
                         textSpacer + sizeMain.Height + e.Bounds.Top + TextureListIconPadding);

        }
    }
}