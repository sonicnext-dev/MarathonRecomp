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
using System.Reflection;
using RageLib.Common;
using RageLib.FileSystem.RPF;
using Directory=RageLib.FileSystem.Common.Directory;
using File=RageLib.FileSystem.RPF.File;

namespace RageLib.FileSystem
{
    public class RPFFileSystem : Common.FileSystem
    {
        private static readonly Dictionary<uint, string> _knownFilenames;

        private File _rpfFile;

        private const bool AssumeVer3FilesAsAudio = true;
        private const string AudioFilesExtension = "ivaud";

        static RPFFileSystem()
        {
            _knownFilenames = new Dictionary<uint, string>();
            using (var s = Assembly.GetExecutingAssembly().GetManifestResourceStream("RageLib.FileSystem.RPF.KnownFilenames.txt"))
            {
                var sw = new StreamReader(s);

                string name;
                while ((name = sw.ReadLine()) != null)
                {
                    uint hash = Hasher.Hash(name);
                    if (!_knownFilenames.ContainsKey(hash))
                    {
                        _knownFilenames.Add(hash, name);
                    }
                }
            }
        }

        public override void Open(string filename)
        {
            _rpfFile = new File();
            if (!_rpfFile.Open(filename))
            {
                throw new Exception("Could not open RPF file.");
            }

            BuildFS();
        }

        public override void Save()
        {
            _rpfFile.Save();
        }

        public override void Rebuild()
        {
            throw new NotImplementedException();
        }

        public override void Close()
        {
            _rpfFile.Close();
        }

        public override bool SupportsRebuild
        {
            get { return false; }
        }

        public override bool HasDirectoryStructure
        {
            get { return true; }
        }

        private string GetName(TOCEntry entry)
        {
            string name;
            if (_rpfFile.Header.Identifier < MagicId.Version3)
            {
                name = _rpfFile.TOC.GetName(entry.NameOffset);
            }
            else
            {
                if (entry == _rpfFile.TOC[0])
                {
                    name = "/";
                }
                else if (_knownFilenames.ContainsKey((uint)entry.NameOffset))
                {
                    name = _knownFilenames[(uint)entry.NameOffset];
                }
                else
                {
                    name = string.Format("0x{0:x}", entry.NameOffset);   
                }

                if (AssumeVer3FilesAsAudio && entry is FileEntry)
                {
                    name += "." + AudioFilesExtension;
                }
            }
            return name;
        }

        private byte[] LoadData(FileEntry entry)
        {
            if (entry.CustomData == null)
            {

                byte[] data = _rpfFile.ReadData(entry.Offset, entry.SizeInArchive);

                if (entry.IsCompressed)
                {
                    data = DataUtil.DecompressDeflate(data, entry.Size);
                }

                return data;
            }
            else
            {
                return entry.CustomData;
            }
        }

        private void StoreData(FileEntry entry, byte[] data)
        {
            entry.SetCustomData(data);
        }


        private void BuildFSDirectory(DirectoryEntry dirEntry, Directory fsDirectory)
        {
            fsDirectory.Name = GetName(dirEntry);

            for (int i = 0; i < dirEntry.ContentEntryCount; i++)
            {
                TOCEntry entry = _rpfFile.TOC[dirEntry.ContentEntryIndex + i];
                if (entry.IsDirectory)
                {
                    var dir = new Directory();

                    BuildFSDirectory(entry as DirectoryEntry, dir);

                    dir.ParentDirectory = fsDirectory;

                    fsDirectory.AddObject(dir);
                }
                else
                {
                    var fileEntry = entry as FileEntry;
                    Common.File.DataLoadDelegate load = () => LoadData(fileEntry);
                    Common.File.DataStoreDelegate store = data => StoreData(fileEntry, data);
                    Common.File.DataIsCustomDelegate isCustom = () => fileEntry.CustomData != null;

                    var file = new Common.File(load, store, isCustom);
                    file.CompressedSize = fileEntry.SizeInArchive;
                    file.IsCompressed = fileEntry.IsCompressed;
                    file.Name = GetName(fileEntry);
                    file.Size = fileEntry.Size;
                    file.IsResource = fileEntry.IsResourceFile;
                    file.ResourceType = fileEntry.ResourceType;
                    file.ParentDirectory = fsDirectory;

                    fsDirectory.AddObject(file);
                }
            }
        }

        private void BuildFS()
        {
            RootDirectory = new Directory();

            TOCEntry entry = _rpfFile.TOC[0];
            BuildFSDirectory(entry as DirectoryEntry, RootDirectory);
        }
    }
}