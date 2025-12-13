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

namespace RageLib.FileSystem.Real
{
    class DirectoryEntry : RealEntry
    {
        private readonly DirectoryInfo _directory;
        private readonly DirectoryInfo[] _subdirs;
        private readonly FileInfo[] _files;

        public DirectoryEntry(RealContext context, DirectoryInfo directory)
        {
            Context = context;
            _directory = directory;

            _subdirs = directory.GetDirectories();
            _files = directory.GetFiles();
        }

        public override bool IsDirectory
        {
            get { return true; }
        }

        public override string Name
        {
            get { return _directory.Name; }
        }

        public int DirectoryCount
        {
            get
            {
                return _subdirs.Length;
            }
        }

        public DirectoryEntry GetDirectory(int index)
        {
            return new DirectoryEntry(Context, _subdirs[index]);
        }

        public int FileCount
        {
            get
            {
                return _files.Length;
            }
        }

        public FileEntry GetFile(int index)
        {
            return new FileEntry(Context, _files[index]);
        }
    }
}
