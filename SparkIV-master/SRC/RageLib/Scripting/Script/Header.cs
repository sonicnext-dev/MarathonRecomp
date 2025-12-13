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

namespace RageLib.Scripting.Script
{
    internal class Header : IFileAccess
    {
        public uint Magic = 0x0d524353;
        public uint MagicEncrypted = 0x0e726373;
        public uint MagicEncryptedCompressed = 0x0e726353;

        public Header(File file)
        {
            File = file;
        }

        public uint Identifier { get; set; }
        public int CodeSize { get; set; }
        public int LocalVarCount { get; set; }
        public int GlobalVarCount { get; set; }
        public int ScriptFlags { get; set; } // 
        public int GlobalsSignature { get; set; } // some hash definitely.. always seems to be 0x7DD1E61C for normal files
                                          // 0x31B42CB2 for navgen_main.sco
        public int CompressedSize { get; set; }

        public File File { get; set; }

        #region IFileAccess Members

        public void Read(BinaryReader br)
        {
            Identifier = br.ReadUInt32();
            CodeSize = br.ReadInt32();
            LocalVarCount = br.ReadInt32();
            GlobalVarCount = br.ReadInt32();
            ScriptFlags = br.ReadInt32();
            GlobalsSignature = br.ReadInt32();

            if (Identifier == MagicEncryptedCompressed)
            {
                CompressedSize = br.ReadInt32();
            }
        }

        public void Write(BinaryWriter bw)
        {
        }

        #endregion
    }
}