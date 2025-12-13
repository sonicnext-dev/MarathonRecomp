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

namespace RageLib.FileSystem.RPF
{
    internal class TOC : IFileAccess, IEnumerable<TOCEntry>
    {
        private readonly List<TOCEntry> _entries = new List<TOCEntry>();
        private string _nameStringTable;

        public TOC(File file)
        {
            File = file;
        }

        public File File { get; private set; }

        public TOCEntry this[int index]
        {
            get { return _entries[index]; }
        }

        public string GetName(int offset)
        {
            if (offset > _nameStringTable.Length)
            {
                throw new Exception("Invalid offset for name");
            }

            int endOffset = offset;
            while (_nameStringTable[endOffset] != 0)
            {
                endOffset++;
            }
            return _nameStringTable.Substring(offset, endOffset - offset);
        }

        #region IFileAccess Members

        public void Read(BinaryReader br)
        {
            if (File.Header.Encrypted)
            {
                int tocSize = File.Header.TOCSize;
                byte[] tocData = br.ReadBytes(tocSize);

                tocData = DataUtil.Decrypt(tocData);

                // Create a memory stream and override our active binary reader
                var ms = new MemoryStream(tocData);
                br = new BinaryReader(ms);
            }

            int entryCount = File.Header.EntryCount;
            for (int i = 0; i < entryCount; i++)
            {
                TOCEntry entry;
                if (TOCEntry.ReadAsDirectoryEntry(br))
                {
                    entry = new DirectoryEntry(this);
                }
                else
                {
                    entry = new FileEntry(this);
                }
                entry.Read(br);
                _entries.Add(entry);
            }

            int stringDataSize = File.Header.TOCSize - File.Header.EntryCount*16;
            byte[] stringData = br.ReadBytes(stringDataSize);
            _nameStringTable = Encoding.ASCII.GetString(stringData);
        }

        public void Write(BinaryWriter bw)
        {
            foreach (var entry in _entries)
            {
                entry.Write(bw);
            }

            byte[] stringData = Encoding.ASCII.GetBytes(_nameStringTable);
            bw.Write(stringData);
        }

        #endregion

        #region Implementation of IEnumerable

        public IEnumerator<TOCEntry> GetEnumerator()
        {
            return _entries.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        #endregion
    }
}