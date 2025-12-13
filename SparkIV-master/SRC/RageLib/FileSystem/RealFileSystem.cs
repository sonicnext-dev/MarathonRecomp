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
using System.Collections.Generic;
using System.IO;
using RageLib.FileSystem.Real;
using Directory=RageLib.FileSystem.Common.Directory;
using File=RageLib.FileSystem.Common.File;

namespace RageLib.FileSystem
{
    public class RealFileSystem : Common.FileSystem
    {
        private RealContext _context;

        // TODO: this has to be refactored to be part of Real.FileEntry
        private readonly Dictionary<string, byte[]> _customData = new Dictionary<string, byte[]>();
        private string _realDirectory;

        public override void Open(string filename)
        {
            _realDirectory = filename;
            _context = new RealContext(new DirectoryInfo(filename));

            BuildFS();
        }

        public override void Save()
        {
            foreach (var pair in _customData)
            {
                System.IO.File.WriteAllBytes(pair.Key, pair.Value);
            }
            _customData.Clear();
        }

        public override void Rebuild()
        {
            throw new NotImplementedException();
        }

        public override void Close()
        {
        }

        public override bool SupportsRebuild
        {
            get { return false; }
        }

        public override bool HasDirectoryStructure
        {
            get { return true; }
        }

        public string RealDirectory
        {
            get { return _realDirectory; }
        }

        private void BuildFSDirectory(DirectoryEntry dirEntry, Directory fsDirectory)
        {
            fsDirectory.Name = dirEntry.Name;

            for (var i = 0; i < dirEntry.DirectoryCount; i++ )
            {
                var dir = new Directory();
                BuildFSDirectory(dirEntry.GetDirectory(i), dir);
                dir.ParentDirectory = fsDirectory;
                fsDirectory.AddObject(dir);
            }

            for (var i = 0; i < dirEntry.FileCount; i++ )
            {
                var fileEntry = dirEntry.GetFile(i);

                File file;
                file = new File( 
                                ()=> (_customData.ContainsKey(fileEntry.FullName) ? _customData[fileEntry.FullName] : fileEntry.GetData()),     
                                data => _customData[fileEntry.FullName] = data,
                                () => _customData.ContainsKey(fileEntry.FullName)
                            );

                file.CompressedSize = fileEntry.Size;
                file.IsCompressed = false;
                file.Name = fileEntry.Name;
                file.Size = fileEntry.Size;
                file.IsResource = fileEntry.IsResourceFile;
                file.ResourceType = fileEntry.ResourceType;
                file.ParentDirectory = fsDirectory;

                fsDirectory.AddObject(file);                
            }
        }

        private void BuildFS()
        {
            RootDirectory = new Directory();

            BuildFSDirectory(_context.RootDirectory, RootDirectory);
        }
    }
}