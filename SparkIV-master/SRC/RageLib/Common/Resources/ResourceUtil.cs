/**********************************************************************\

 RageLib
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
using System.IO;
using System.Text;

namespace RageLib.Common.Resources
{
    public static class ResourceUtil
    {
        public static bool IsResource(Stream stream)
        {
            var rh = new ResourceHeader();
            rh.Read(new BinaryReader(stream));
            return rh.Magic == ResourceHeader.MagicValue;
        }

        public static void GetResourceData(Stream stream, out uint flags, out ResourceType type)
        {
            var rh = new ResourceHeader();
            rh.Read(new BinaryReader(stream));
            flags = rh.Flags;
            type = rh.Type;
        }

        public static uint ReadOffset(BinaryReader br)
        {
            uint value;
            uint offset = br.ReadUInt32();
            
            if (offset == 0)
            {
                value = 0;
            }
            else
            {
                if (offset >> 28 != 5)
                {
                    throw new Exception("Expected an offset.");
                }
                value = offset & 0x0fffffff;
            }

            return value;
        }

        public static uint ReadDataOffset(BinaryReader br)
        {
            uint value;
            uint offset = br.ReadUInt32();

            if (offset == 0)
            {
                value = 0;
            }
            else
            {
                if (offset >> 28 != 6)
                {
                    throw new Exception("Expected a data offset.");
                }
                value = offset & 0x0fffffff;
            }

            return value;
        }

        public static uint ReadDataOffset(BinaryReader br, uint mask, out uint lowerBits)
        {
            uint value;
            uint offset = br.ReadUInt32();

            if (offset == 0)
            {
                lowerBits = 0;
                value = 0;
            }
            else
            {
                if (offset >> 28 != 6)
                {
                    throw new Exception("Expected a data offset.");
                }
                value = offset & mask;
                lowerBits = offset & (~mask & 0xff);
            }

            return value;
        }

        public static string ReadNullTerminatedString(BinaryReader br)
        {
            var sb = new StringBuilder();

            var c = (char) br.ReadByte();
            while (c != 0)
            {
                sb.Append(c);
                c = (char) br.ReadByte();
            }

            return sb.ToString();
        }

    }
}