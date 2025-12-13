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

using System.IO;

namespace RageLib.Common.Resources
{
    internal class ResourceHeader
    {
        private const uint MagicBigEndian = 0x52534305;
        public const uint MagicValue = 0x05435352;

        public uint Magic { get; set; }
        public ResourceType Type { get; set; }
        public uint Flags { get; set; }
        public CompressionType CompressCodec { get; set; }

        public int GetSystemMemSize()
        {
            return (int)(Flags & 0x7FF) << (int)(((Flags >> 11) & 0xF) + 8);
        }

        public int GetGraphicsMemSize()
        {
            return (int)((Flags >> 15) & 0x7FF) << (int)(((Flags >> 26) & 0xF) + 8);
        }

        public void SetMemSizes(int systemMemSize, int graphicsMemSize)
        {
            // gfx = a << (b + 8)
            // minimum representable is block of 0x100 bytes

            const int maxA = 0x3F;

            int sysA = systemMemSize >> 8;
            int sysB = 0;

            while(sysA > maxA)
            {
                if ((sysA & 1) != 0)
                {
                    sysA += 2;
                }
                sysA >>= 1;
                sysB++;
            }

            int gfxA = graphicsMemSize >> 8;
            int gfxB = 0;

            while (gfxA > maxA)
            {
                if ((gfxA & 1) != 0)
                {
                    gfxA += 2;
                }
                gfxA >>= 1;
                gfxB++;
            }
            
            Flags = (Flags & 0xC0000000) | (uint)(sysA | (sysB << 11) | (gfxA << 15) | (gfxB << 26));
        }

        public void Read(BinaryReader br)
        {
            Magic = br.ReadUInt32();
            Type = (ResourceType) br.ReadUInt32();
            Flags = br.ReadUInt32();
            CompressCodec = (CompressionType)br.ReadUInt16();

            if (Magic == MagicBigEndian)
            {
                Magic = DataUtil.SwapEndian(Magic);
                Type = (ResourceType)DataUtil.SwapEndian((uint)Type);
                Flags = DataUtil.SwapEndian(Flags);
            }
        }

        public void Write(BinaryWriter bw)
        {
            bw.Write( MagicValue );
            bw.Write( (uint)Type );
            bw.Write( Flags );
            bw.Write( (ushort)CompressCodec );
        }
    }
}