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
using RageLib.Textures.Decoder;
using RageLib.Textures.Encoder;
using RageLib.Textures.Resource;

namespace RageLib.Textures
{
    public class Texture : IDisposable
    {
        public const int ThumbnailSize = 32;
        
        private Image _thumbnailCache;

        internal Texture(TextureInfo info)
        {
            Name = info.Name;
            Width = info.Width;
            Height = info.Height;

            switch (info.Format)
            {
                case D3DFormat.DXT1:
                    TextureType = TextureType.DXT1;
                    break;
                case D3DFormat.DXT3:
                    TextureType = TextureType.DXT3;
                    break;
                case D3DFormat.DXT5:
                    TextureType = TextureType.DXT5;
                    break;
                case D3DFormat.A8R8G8B8:
                    TextureType = TextureType.A8R8G8B8;
                    break;
                case D3DFormat.L8:
                    TextureType = TextureType.L8;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            Levels = info.Levels;
            Info = info;
        }

        internal TextureInfo Info { get; set; }

        public uint Width { get; private set; }
        public uint Height { get; private set; }
        public TextureType TextureType { get; private set; }
        public byte[] TextureData { get { return Info.TextureData; } }
        public string Name { get; private set; }
        public int Levels { get; private set; }

        public string TitleName
        { 
            get
            {
                string name = Name;
                if (name.StartsWith("pack:/"))
                {
                    name = Name.Substring(6);
                }
                if (name.EndsWith(".dds"))
                {
                    name = name.Substring(0, name.Length - 4);
                }                
                return name;
            }
        }

        public Image DecodeAsThumbnail()
        {
            if (_thumbnailCache == null)
            {
                Image image = Decode();

                int thumbWidth = ThumbnailSize;
                int thumbHeight = ThumbnailSize;
                if (Width > Height)
                {
                    thumbHeight = (int)Math.Ceiling(((float) Height/Width)*ThumbnailSize);
                }
                else if (Height > Width)
                {
                    thumbWidth = (int)Math.Ceiling(((float) Width/Height)*ThumbnailSize);
                }

                if (Environment.OSVersion.Version.Major >= 6 && Environment.OSVersion.Version.Minor >= 1)
                {
                    // for Windows 7
                    // Don't use GetThumbnailImage as GDI+ is bugged.

                    _thumbnailCache = new Bitmap(thumbWidth, thumbHeight);
                    using (var g = Graphics.FromImage(_thumbnailCache))
                    {
                        g.SmoothingMode = SmoothingMode.HighQuality;
                        g.InterpolationMode = InterpolationMode.HighQualityBilinear;
                        g.DrawImage(image, 0, 0, thumbWidth, thumbHeight);
                    }
                }
                else
                {
                    _thumbnailCache = image.GetThumbnailImage(thumbWidth, thumbHeight, () => false, IntPtr.Zero);                    
                }
            }

            return _thumbnailCache;
        }

        public Image Decode()
        {
            return Decode(0);
        }

        public Image Decode(int level)
        {
            return TextureDecoder.Decode(this, level);
        }

        public void Encode(Image image)
        {
            for(int i=0; i<Levels; i++)
            {
                TextureEncoder.Encode(this, image, i);
            }

            if (_thumbnailCache != null)
            {
                _thumbnailCache.Dispose();
                _thumbnailCache = null;
            }
        }

        public override string ToString()
        {
            return string.Format("{0} ({1}x{2} {3})", Name, Width, Height, TextureType);
        }

        public uint GetTextureDataSize(int level)
        {
            var width = GetWidth(level);
            var height = GetHeight(level);
            uint size;

            switch(TextureType)
            {
                case TextureType.DXT1:
                    size = (width*height)/2;
                    break;
                case TextureType.DXT3:
                case TextureType.DXT5:
                    size = width*height;
                    break;
                case TextureType.A8R8G8B8:
                    size = width*height*4;
                    break;
                case TextureType.L8:
                    size = width*height;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            return size;
        }

        public uint GetWidth(int level)
        {
            return Width/(uint)Math.Pow(2, level);
        }

        public uint GetHeight(int level)
        {
            return Height / (uint)Math.Pow(2, level);
        }

        public byte[] GetTextureData(int level)
        {
            byte[] data;
            if (level == 0)
            {
                data = TextureData;
            }
            else
            {
                uint offset = 0;
                for(var i=0; i<level; i++)
                {
                    offset += GetTextureDataSize(i);
                }
                var size = GetTextureDataSize(level);

                data = new byte[size];
                Array.Copy(TextureData, offset, data, 0, size);
            }
            return data;
        }

        public void SetTextureData(int level, byte[] data)
        {
            uint offset = 0;
            for (var i = 0; i < level; i++)
            {
                offset += GetTextureDataSize(i);
            }
            var size = GetTextureDataSize(level);

            Array.Copy(data, 0, TextureData, offset, size);
        }

        #region Implementation of IDisposable

        public void Dispose()
        {
            Info = null;

            if (_thumbnailCache != null)
            {
                _thumbnailCache.Dispose();
                _thumbnailCache = null;                
            }
        }

        #endregion
    }
}