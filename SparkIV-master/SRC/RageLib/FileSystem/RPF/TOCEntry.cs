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
    internal abstract class TOCEntry : IFileAccess
    {
        public int NameOffset { get; set; }
        public TOC TOC { get; set; }

        public abstract bool IsDirectory { get; }

        #region IFileAccess Members

        public abstract void Read(BinaryReader br);
        public abstract void Write(BinaryWriter bw);

        #endregion

        internal static bool ReadAsDirectoryEntry(BinaryReader br)
        {
            bool dir;

            br.BaseStream.Seek(8, SeekOrigin.Current);
            dir = br.ReadInt32() < 0;
            br.BaseStream.Seek(-12, SeekOrigin.Current);

            return dir;
        }
    }
}