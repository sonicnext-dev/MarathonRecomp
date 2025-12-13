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

namespace RageLib.Textures.Filter
{
    class ChannelFilter : IFilter
    {
        private ImageChannel _channel;

        public ChannelFilter(ImageChannel channel)
        {
            _channel = channel;
        }

        public void Apply(Image image)
        {
            if (_channel != ImageChannel.All)
            {
                uint mask;
                int shift;

                switch (_channel)
                {
                    case ImageChannel.Red:
                        mask = 0x00FF0000;
                        shift = 16;
                        break;
                    case ImageChannel.Green:
                        mask = 0x0000FF00;
                        shift = 8;
                        break;
                    case ImageChannel.Blue:
                        mask = 0x000000FF;
                        shift = 0;
                        break;
                    case ImageChannel.Alpha:
                        mask = 0xFF000000;
                        shift = 24;
                        break;
                    default:
                        throw new ArgumentOutOfRangeException();
                }

                var bmp = image as Bitmap;

                if (bmp != null)
                {
                    var rect = new Rectangle(0, 0, image.Width, image.Height);
                    var bmpdata = bmp.LockBits(rect, ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);

                    unsafe
                    {
                        var p = (byte*)bmpdata.Scan0;
                        for(var y = 0; y < bmp.Height; y++)
                        {
                            for(var x = 0; x < bmp.Width; x++)
                            {
                                var offset = y*bmpdata.Stride + x*4;
                                var data = (byte)(((*(int*) (p + offset)) & mask) >> shift);
                                p[offset + 0] = data;
                                p[offset + 1] = data;
                                p[offset + 2] = data;
                                p[offset + 3] = 255;
                            }
                        }
                    }

                    bmp.UnlockBits(bmpdata);
                }

            }
        }
    }
}
