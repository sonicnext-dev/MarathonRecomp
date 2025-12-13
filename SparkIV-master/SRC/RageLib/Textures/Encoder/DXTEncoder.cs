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

namespace RageLib.Textures.Encoder
{
    internal static class DXTEncoder
    {
        public static byte[] EncodeDXT1(byte[] data, int width, int height)
        {
            var dataSize = (width / 4) * (height / 4) * 8;
            var outData = new byte[dataSize];
            Squish.CompressImage(data, (uint)width, (uint)height, outData, (int)Squish.Flags.DXT1);

            return outData;
        }

        public static byte[] EncodeDXT3(byte[] data, int width, int height)
        {
            var dataSize = (width / 4) * (height / 4) * 16;
            var outData = new byte[dataSize];
            Squish.CompressImage(data, (uint)width, (uint)height, outData, (int)Squish.Flags.DXT3);

            return outData;
        }

        public static byte[] EncodeDXT5(byte[] data, int width, int height)
        {
            var dataSize = (width / 4) * (height / 4) * 16;
            var outData = new byte[dataSize];
            Squish.CompressImage(data, (uint)width, (uint)height, outData, (int)Squish.Flags.DXT5);

            return outData;
        }
    }
}
