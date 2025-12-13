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
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using RageLib.Common;

namespace RageLib.FileSystem.IMG
{
    internal class TOC : IFileAccess, IEnumerable<TOCEntry>
    {
        public const int EntrySize = 16;

        private readonly List<TOCEntry> _entries = new List<TOCEntry>();
        private string[] _nameTable;

        public TOC(File file)
        {
            File = file;
        }

        public File File { get; private set; }

        public TOCEntry this[int index]
        {
            get { return _entries[index]; }
        }

        public string GetName(int index)
        {
            return _nameTable[index];
        }

        public int GetTOCBlockSize()
        {
            int size = _entries.Count*EntrySize;
            foreach (var s in _nameTable)
            {
                size += s.Length + 1;
            }
            return (int)Math.Ceiling((float)size / TOCEntry.BlockSize);
        }

        #region IFileAccess Members

        public void Read(BinaryReader br)
        {
            int entryCount = File.Header.EntryCount;
            for (int i = 0; i < entryCount; i++)
            {
                var entry = new TOCEntry(this);
                entry.Read(br);
                _entries.Add(entry);
            }

            int stringDataSize = File.Header.TocSize - File.Header.EntryCount * EntrySize;
            byte[] stringData = br.ReadBytes(stringDataSize);
            string nameStringTable = Encoding.ASCII.GetString(stringData);
            _nameTable = nameStringTable.Split((char) 0);
        }

        public void Write(BinaryWriter bw)
        {
            foreach (var entry in _entries)
            {
                entry.Write(bw);
            }

            foreach (var s in _nameTable)
            {
                byte[] nameData = Encoding.ASCII.GetBytes(s);
                bw.Write( nameData );
                bw.Write( (byte)0 );
            }
        }

        #endregion

        #region Implementation of IEnumerable

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        #endregion

        #region Implementation of IEnumerable<TOC>

        public IEnumerator<TOCEntry> GetEnumerator()
        {
            return _entries.GetEnumerator();
        }

        #endregion
    }
}