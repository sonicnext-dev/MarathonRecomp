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

namespace RageLib.FileSystem.RPF
{
    internal class Header : IFileAccess
    {
        public Header(File file)
        {
            File = file;
        }

        public MagicId Identifier { get; set; }
        public int TOCSize { get; set; }
        public int EntryCount { get; set; }

        private int Unknown1 { get; set; }
        private int EncryptedFlag { get; set; }

        public File File { get; private set; }

        public bool Encrypted
        {
            get { return EncryptedFlag != 0; }
            set { EncryptedFlag = value ? -1 : 0; }
        }

        #region IFileAccess Members

        public void Read(BinaryReader br)
        {
            Identifier = (MagicId) br.ReadInt32();
            TOCSize = br.ReadInt32();
            EntryCount = br.ReadInt32();
            Unknown1 = br.ReadInt32();
            EncryptedFlag = br.ReadInt32();
        }

        public void Write(BinaryWriter bw)
        {
            bw.Write((int)Identifier);
            bw.Write(TOCSize);
            bw.Write(EntryCount);
            bw.Write(Unknown1);
            bw.Write((int)0);           // not encrypted, we won't write encrypted archives :)
        }

        #endregion
    }
}