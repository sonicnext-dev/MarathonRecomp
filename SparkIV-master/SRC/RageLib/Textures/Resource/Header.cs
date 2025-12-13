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
    internal class Header : IFileAccess
    {
        public uint VTable { get; private set; }

        private uint BlockMapOffset { get; set; }

        private uint ParentDictionary { get; set; } // always 0 in file

        public uint UsageCount { get; private set; } // always 1 in file

        public short TextureCount { get; private set; } // actually NameHash.Count

        public uint TextureListOffset { get; private set; }

        public uint HashTableOffset { get; private set; }

        #region IFileAccess Members

        public void Read(BinaryReader br)
        {
            // Full Structure of rage::pgDictionary

            // rage::datBase
            VTable = br.ReadUInt32();

            // rage::pgBase
            BlockMapOffset = ResourceUtil.ReadOffset(br);
            ParentDictionary = br.ReadUInt32();
            UsageCount = br.ReadUInt32();

            // CSimpleCollection<DWORD>
            HashTableOffset = ResourceUtil.ReadOffset(br);
            TextureCount = br.ReadInt16();
            br.ReadInt16();

            // CPtrCollection<T>
            TextureListOffset = ResourceUtil.ReadOffset(br);
            br.ReadInt16();
            br.ReadInt16();
        }

        public void Write(BinaryWriter bw)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}