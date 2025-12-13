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
using RageLib.Common.Resources;

namespace RageLib.FileSystem.RPF
{
    internal class FileEntry : TOCEntry
    {
        public const int BlockSize = 0x800;

        public FileEntry(TOC toc)
        {
            TOC = toc;
        }

        public int SizeUsed { get; set; }

        public int Size { get; set; } // (ala uncompressed size)
        public int Offset { get; set; }
        public int SizeInArchive { get; set; }
        public bool IsCompressed { get; set; }
        public bool IsResourceFile { get; set; }
        public ResourceType ResourceType { get; set; }
        public uint RSCFlags { get; set; }

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
                SizeInArchive = data.Length;
                IsCompressed = false;

                if ((data.Length % BlockSize) != 0)
                {
                    int fullDataLength = data.Length + (BlockSize - data.Length % BlockSize);
                    var newData = new byte[fullDataLength];
                    data.CopyTo(newData, 0);
                    data = newData;
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

        public override bool IsDirectory
        {
            get { return false; }
        }

        public override void Read(BinaryReader br)
        {
            NameOffset = br.ReadInt32();
            Size = br.ReadInt32();

            Offset = br.ReadInt32();

            uint temp = br.ReadUInt32();

            IsResourceFile = (temp & 0xC0000000) == 0xC0000000;

            if (IsResourceFile)
            {
                ResourceType = (ResourceType) (Offset & 0xFF);
                Offset = Offset & 0x7fffff00;
                SizeInArchive = Size;
                IsCompressed = false;
                RSCFlags = temp;
            }
            else
            {
                SizeInArchive = (int) (temp & 0xbfffffff);
                IsCompressed = (temp & 0x40000000) != 0;
            }

            SizeUsed = (int)Math.Ceiling((float)SizeInArchive / BlockSize) * BlockSize;
        }

        public override void Write(BinaryWriter bw)
        {
            bw.Write(NameOffset);
            bw.Write(Size);

            if (IsResourceFile)
            {
                bw.Write(Offset | (byte)ResourceType);
                bw.Write(RSCFlags);
            }
            else
            {
                bw.Write(Offset);

                var temp = SizeInArchive;
                if (IsCompressed)
                {
                    temp |= 0x40000000;
                }
                bw.Write(temp);
            }
        }
    }
}