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
using RageLib.Common;
using RageLib.Common.Resources;

namespace RageLib.Textures.Resource
{
    internal class TextureInfo : IFileAccess
    {
        public File File { get; set; }

        public uint VTable { get; private set; }

        private uint BlockMapOffset { get; set; } // 0 in file

        private uint Unknown1 { get; set; } // 1 / 0x10000 on PC (really composed of a BYTE, BYTE, WORD)
        private uint Unknown2 { get; set; } // 0
        private uint Unknown3 { get; set; } // 0

        public string Name { get; private set; }
        private uint Unknown4 { get; set; } // 0;   // set in memory by game

        public ushort Width { get; private set; }
        public ushort Height { get; private set; }

        public D3DFormat Format;

        private ushort StrideSize { get; set; }
        private byte Type { get; set; }   // 0 = normal, 1 = cube, 3 = volume
        public byte Levels { get; set; } // MipMap levels

        private float UnknownFloat1 { get; set; } // 1.0f
        private float UnknownFloat2 { get; set; } // 1.0f
        private float UnknownFloat3 { get; set; } // 1.0f
        private float UnknownFloat4 { get; set; } // 0
        private float UnknownFloat5 { get; set; } // 0
        private float UnknownFloat6 { get; set; } // 0

        private uint PrevTextureInfoOffset { get; set; }    // sometimes not always accurate
        private uint NextTextureInfoOffset { get; set; }    // always 0

        internal uint RawDataOffset { get; set; }
        public byte[] TextureData { get; private set; }

        private uint Unknown6 { get; set; }

        public void ReadData(BinaryReader br)
        {
            int dataSize = GetTotalDataSize();

            br.BaseStream.Seek(RawDataOffset, SeekOrigin.Begin);
            TextureData = br.ReadBytes(dataSize);
        }

        internal int GetTotalDataSize()
        {
            uint width = Width;
            uint height = Height;

            int dataSize;
            switch (Format)
            {
                case D3DFormat.DXT1:
                    dataSize = (int) (width*height/2);
                    break;
                case D3DFormat.DXT3:
                case D3DFormat.DXT5:
                    dataSize = (int) (width*height);
                    break;
                case D3DFormat.A8R8G8B8:
                    dataSize = (int) (width*height*4);
                    break;
                case D3DFormat.L8:
                    dataSize = (int) (width*height);
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            int levels = Levels;
            int levelDataSize = dataSize;
            while(levels > 1)
            {
                dataSize += (levelDataSize/4);
                
                levelDataSize /= 4;

                // clamp to 16 bytes
                if (levelDataSize < 16)
                {
                    if (Format == D3DFormat.DXT1 && levelDataSize < 8)
                    {
                        levelDataSize = 8;
                    }
                    else
                    {
                        levelDataSize = 16;
                    }
                }

                levels--;
            }
            return dataSize;
        }

        public void WriteData(BinaryWriter bw)
        {
            bw.BaseStream.Seek(RawDataOffset, SeekOrigin.Begin);
            bw.Write(TextureData);
        }

        #region IFileAccess Members

        public void Read(BinaryReader br)
        {
            // Full structure of rage::grcTexturePC

            // rage::datBase
            VTable = br.ReadUInt32(); 

            // rage::pgBase
            BlockMapOffset = ResourceUtil.ReadOffset(br);
            
            // Texture Info struct:
            Unknown1 = br.ReadUInt32(); // BYTE, BYTE, WORD
            Unknown2 = br.ReadUInt32();
            Unknown3 = br.ReadUInt32();
            
            uint nameOffset = ResourceUtil.ReadOffset(br);
            
            Unknown4 = br.ReadUInt32();

            // Texture Data struct:
            Width = br.ReadUInt16();
            Height = br.ReadUInt16();
            Format = (D3DFormat) br.ReadInt32();

            StrideSize = br.ReadUInt16();
            Type = br.ReadByte();
            Levels = br.ReadByte();

            UnknownFloat1 = br.ReadSingle();
            UnknownFloat2 = br.ReadSingle();
            UnknownFloat3 = br.ReadSingle();
            UnknownFloat4 = br.ReadSingle();
            UnknownFloat5 = br.ReadSingle();
            UnknownFloat6 = br.ReadSingle();

            PrevTextureInfoOffset = ResourceUtil.ReadOffset(br);
            NextTextureInfoOffset = ResourceUtil.ReadOffset(br);

            RawDataOffset = ResourceUtil.ReadDataOffset(br);

            Unknown6 = br.ReadUInt32();

            // Read texture name
            br.BaseStream.Seek(nameOffset, SeekOrigin.Begin);
            Name = ResourceUtil.ReadNullTerminatedString(br);
        }

        public void Write(BinaryWriter bw)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}