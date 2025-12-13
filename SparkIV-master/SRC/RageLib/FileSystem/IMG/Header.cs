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

namespace RageLib.FileSystem.IMG
{
    internal class Header : IFileAccess
    {
        public const uint MagicId = 0xA94E2A52;
        public const int SupportedVersion = 3;

        public Header(File file)
        {
            File = file;
        }

        public uint Identifier { get; set; }
        public int Version { get; set; }
        public int EntryCount { get; set; }
        public int TocSize { get; set; }
        public short TocEntrySize { get; set; }
        private short Unknown2 { get; set; }

        public File File { get; private set; }

        #region IFileAccess Members

        public void Read(BinaryReader br)
        {
            Identifier = br.ReadUInt32();
            Version = br.ReadInt32();
            EntryCount = br.ReadInt32();
            TocSize = br.ReadInt32();
            TocEntrySize = br.ReadInt16();
            Unknown2 = br.ReadInt16();
        }

        public void Write(BinaryWriter bw)
        {
            bw.Write( Identifier );
            bw.Write( Version );
            bw.Write( EntryCount );
            bw.Write( TocSize );
            bw.Write( TocEntrySize );
            bw.Write( Unknown2 );
        }

        #endregion
    }
}