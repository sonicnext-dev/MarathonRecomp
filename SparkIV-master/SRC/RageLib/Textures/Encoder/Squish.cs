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

using System.Runtime.InteropServices;

namespace RageLib.Textures.Encoder
{
    internal class Squish
    {
        [DllImport("libsquish.dll", EntryPoint = "CompressImage")]
        public static extern void CompressImage([MarshalAs(UnmanagedType.LPArray)] byte[] rgba, uint width, uint height, [MarshalAs(UnmanagedType.LPArray)] byte[] blocks, int flags);

        public enum Flags
        {
            DXT1 = 1 << 0,
            DXT3 = 1 << 1,
            DXT5 = 1 << 2,

            ColourIterativeClusterFit = 1 << 8,
            ColourClusterFit = 1 << 3,              // Default
            ColourRangeFit = 1 << 4,

            ColourMetricPerceptual = 1 << 5,        // Default
            ColourMetricUniform = 1 << 6,

            WeightColourByAlpha = 1 << 7,           // Disabled by Default
        }
    }
}
