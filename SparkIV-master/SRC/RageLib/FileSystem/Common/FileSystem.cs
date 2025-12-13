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

using System.Diagnostics;

namespace RageLib.FileSystem.Common
{
    public abstract class FileSystem
    {
        public Directory RootDirectory { get; protected set; }

        public abstract void Open(string filename);

        public abstract void Save();

        public abstract void Rebuild();

        public abstract void Close();

        public abstract bool SupportsRebuild { get; }
        public abstract bool HasDirectoryStructure { get; }

        internal void DumpFSToDebug()
        {
            DumpDirToDebug("", RootDirectory);
        }

        private static void DumpDirToDebug(string indent, Directory dir)
        {
            Debug.WriteLine(indent + dir.Name);
            indent += "  ";
            foreach (FSObject item in dir)
            {
                if (item.IsDirectory)
                {
                    DumpDirToDebug(indent, item as Directory);
                }
                else
                {
                    var file = item as File;
                    Debug.WriteLine(indent + item.Name + "   (Size: " + file.Size + ", Compressed: " + file.IsCompressed +
                                    ")");
                }
            }
        }
    }
}