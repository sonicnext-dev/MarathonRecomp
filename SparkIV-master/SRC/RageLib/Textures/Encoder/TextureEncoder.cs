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
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

namespace RageLib.Textures.Encoder
{
    internal class TextureEncoder
    {
        internal static void Encode(Texture texture, Image image, int level)
        {
            var width = texture.GetWidth(level);
            var height = texture.GetHeight(level);
            var data = new byte[width * height * 4];  // R G B A

            var bitmap = new Bitmap((int)width, (int)height);

            Graphics g = Graphics.FromImage(bitmap);
            g.InterpolationMode = InterpolationMode.HighQualityBilinear;
            g.DrawImage(image, 0, 0, (int)width, (int)height);
            g.Dispose();

            var rect = new Rectangle(0, 0, (int) width, (int) height);
            BitmapData bmpdata = bitmap.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);

            if (texture.TextureType == TextureType.A8R8G8B8)
            {
                Marshal.Copy(bmpdata.Scan0, data, 0, (int) width*(int) height*4);
            }
            else if (texture.TextureType == TextureType.L8)
            {
                var newData = new byte[width * height];

                // Convert to L8
                unsafe
                {
                    var p = (byte*)bmpdata.Scan0;
                    for (var y = 0; y < bitmap.Height; y++)
                    {
                        for (var x = 0; x < bitmap.Width; x++)
                        {
                            var offset = y * bmpdata.Stride + x * 4;
                            var dataOffset = y * width + x;

                            newData[dataOffset] = (byte)((p[offset + 2] + p[offset + 1] + p[offset + 0])/3);
                        }
                    }
                }

                data = newData;
            }
            else
            {
                // Convert from the B G R A format stored by GDI+ to R G B A
                unsafe
                {
                    var p = (byte*)bmpdata.Scan0;
                    for (var y = 0; y < bitmap.Height; y++)
                    {
                        for (var x = 0; x < bitmap.Width; x++)
                        {
                            var offset = y * bmpdata.Stride + x * 4;
                            var dataOffset = y * width * 4 + x * 4;
                            data[dataOffset + 0] = p[offset + 2];       // R
                            data[dataOffset + 1] = p[offset + 1];       // G
                            data[dataOffset + 2] = p[offset + 0];       // B
                            data[dataOffset + 3] = p[offset + 3];       // A
                        }
                    }
                }
            }

            bitmap.UnlockBits(bmpdata);

            bitmap.Dispose();

            switch (texture.TextureType)
            {
                case TextureType.DXT1:
                    data = DXTEncoder.EncodeDXT1(data, (int) width, (int) height);
                    break;
                case TextureType.DXT3:
                    data = DXTEncoder.EncodeDXT3(data, (int) width, (int) height);
                    break;
                case TextureType.DXT5:
                    data = DXTEncoder.EncodeDXT5(data, (int) width, (int) height);
                    break;
                case TextureType.A8R8G8B8:
                case TextureType.L8:
                    // Nothing to do
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            texture.SetTextureData(level, data);
        }
    }
}
