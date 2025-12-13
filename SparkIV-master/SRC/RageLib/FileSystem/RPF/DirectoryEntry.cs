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

namespace RageLib.FileSystem.RPF
{
    internal class DirectoryEntry : TOCEntry
    {
        public DirectoryEntry(TOC toc)
        {
            TOC = toc;
        }

        public int Flags { get; set; }
        public int ContentEntryIndex { get; set; }
        public int ContentEntryCount { get; set; }

        public override bool IsDirectory
        {
            get { return true; }
        }

        public override void Read(BinaryReader br)
        {
            NameOffset = br.ReadInt32();
            Flags = br.ReadInt32();
            ContentEntryIndex = (int) (br.ReadUInt32() & 0x7fffffff);
            ContentEntryCount = br.ReadInt32() & 0x0fffffff;
        }

        public override void Write(BinaryWriter bw)
        {
            bw.Write(NameOffset);
            bw.Write(Flags);

            uint temp = (uint)ContentEntryIndex | 0x80000000;
            bw.Write(temp);
            bw.Write(ContentEntryCount);
        }
    }
}