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

namespace RageLib.Common.ResourceTypes
{
    // pgDictionary<T>
    public class PGDictionary<T> : PGBase, IFileAccess where T: class, IFileAccess, new()
    {
        private uint ParentDictionary { get; set; } // always 0 in file

        public uint UsageCount { get; private set; } // always 1 in file

        public SimpleCollection<uint> NameHashes { get; private set; }

        public PtrCollection<T> Entries { get; private set; }

        #region IFileAccess Members

        public new void Read(BinaryReader br)
        {
            base.Read(br);
            
            ParentDictionary = br.ReadUInt32();
            UsageCount = br.ReadUInt32();

            // CSimpleCollection<DWORD>
            NameHashes = new SimpleCollection<uint>(br, reader => reader.ReadUInt32());

            // CPtrCollection<T>
            Entries = new PtrCollection<T>(br);
        }

        public new void Write(BinaryWriter bw)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}