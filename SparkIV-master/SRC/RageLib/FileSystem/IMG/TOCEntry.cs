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
using RageLib.Common;
using RageLib.Common.Resources;

namespace RageLib.FileSystem.IMG
{
    internal class TOCEntry : IFileAccess
    {
        public const int BlockSize = 0x800;

        public TOCEntry(TOC toc)
        {
            TOC = toc;
        }

        public int Size { get; set; } // For normal entries, this is the real size, for RSC, this is computed.
        public uint RSCFlags { get; set; } // For RSC entries
        public ResourceType ResourceType { get; set; }
        public int OffsetBlock { get; set; }
        public short UsedBlocks { get; set; }
        public short Flags { get; set; }

        public int PaddingCount
        {
            get
            {
                return Flags & 0x7FF;
            }
            set
            {
                Flags = (short)((Flags & ~0x7FF) | value);
            }
        }

        public bool IsResourceFile { get; set; }

        public TOC TOC { get; set; }

        public byte[] CustomData { get; private set; }

        public void SetCustomData(byte[] data)
        {
            if (data == null)
            {
                CustomData = null;
            }
            else
            {
                Size = data.Length;

                if ((data.Length % BlockSize) != 0)
                {
                    int padding = (BlockSize - data.Length%BlockSize);
                    int fullDataLength = data.Length + padding;
                    var newData = new byte[fullDataLength];
                    data.CopyTo(newData, 0);
                    data = newData;

                    PaddingCount = padding;
                }
                else
                {
                    PaddingCount = 0;
                }

                CustomData = data;

                if (IsResourceFile)
                {
                    var ms = new MemoryStream(data, false);

                    uint flags;
                    ResourceType resType;

                    ResourceUtil.GetResourceData(ms, out flags, out resType);

                    RSCFlags = flags;
                    ResourceType = resType;

                    ms.Close();
                }
            }
        }

        #region IFileAccess Members

        public void Read(BinaryReader br)
        {
            uint temp = br.ReadUInt32();
            IsResourceFile = ((temp & 0xc0000000) != 0);

            if (!IsResourceFile)
            {
                Size = (int) temp;
            }
            else
            {
                RSCFlags = temp;
            }

            ResourceType = (ResourceType) br.ReadInt32();
            OffsetBlock = br.ReadInt32();
            UsedBlocks = br.ReadInt16();
            Flags = br.ReadInt16();

            if (IsResourceFile)
            {
                Size = UsedBlocks*0x800 - PaddingCount;
            }

            // Uses 0x4000 on Flags to determine if its old style resources
            // if its not 0, its old style!

            // Uses 0x2000 on Flags to determine if its a RSC,
            // if its 1, its a RSC!
        }

        public void Write(BinaryWriter bw)
        {
            if (!IsResourceFile)
            {
                bw.Write( Size );
            }
            else
            {
                bw.Write( RSCFlags );
            }

            bw.Write( (int)ResourceType );
            bw.Write( OffsetBlock );
            bw.Write( UsedBlocks );
            bw.Write( Flags );
        }

        #endregion
    }
}