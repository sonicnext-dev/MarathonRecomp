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
using System.Runtime.InteropServices;

namespace RageLib.Textures.Decoder
{
    internal class TextureDecoder
    {
        internal static Image Decode(Texture texture, int level)
        {
            var width = texture.GetWidth(level);
            var height = texture.GetHeight(level);
            var data = texture.GetTextureData(level);
            
            switch(texture.TextureType)
            {
                case TextureType.DXT1:
                    data = DXTDecoder.DecodeDXT1(data, (int)width, (int)height);
                    break;
                case TextureType.DXT3:
                    data = DXTDecoder.DecodeDXT3(data, (int)width, (int)height);
                    break;
                case TextureType.DXT5:
                    data = DXTDecoder.DecodeDXT5(data, (int)width, (int)height);
                    break;
                case TextureType.A8R8G8B8:
                    // Nothing to do, the data is already in the format we want it to be
                    break;
                case TextureType.L8:
                    {
                        var newData = new byte[data.Length*4];
                        for (int i = 0; i < data.Length; i++)
                        {
                            newData[i*4 + 0] = data[i];
                            newData[i*4 + 1] = data[i];
                            newData[i*4 + 2] = data[i];
                            newData[i*4 + 3] = 255;
                        }
                        data = newData;
                    }
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            var bmp = new Bitmap((int) width, (int) height, PixelFormat.Format32bppArgb);

            var rect = new Rectangle(0, 0, (int) width, (int) height);
            var bmpdata = bmp.LockBits(rect, ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);

            Marshal.Copy(data, 0, bmpdata.Scan0, (int) width*(int) height*4);
            
            bmp.UnlockBits(bmpdata);

            return bmp;
        }
    }
}
