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
using RageLib.FileSystem.Common;
using RageLib.FileSystem.IMG;
using File=RageLib.FileSystem.IMG.File;

namespace RageLib.FileSystem
{
    public class IMGFileSystem : Common.FileSystem
    {
        private File _imgFile;

        public override void Open(string filename)
        {
            _imgFile = new File();
            if (!_imgFile.Open(filename))
            {
                throw new Exception("Could not open IMG file.");
            }

            BuildFS();
        }

        public override void Save()
        {
            _imgFile.Save();
        }

        public override void Rebuild()
        {
            _imgFile.Rebuild();
        }

        public override void Close()
        {
            _imgFile.Close();
        }

        public override bool SupportsRebuild
        {
            get { return true; }
        }

        public override bool HasDirectoryStructure
        {
            get { return false; }
        }

        private byte[] LoadData(TOCEntry entry)
        {
            if (entry.CustomData == null)
            {
                byte[] data = _imgFile.ReadData(entry.OffsetBlock * 0x800, entry.Size);

                return data;                
            }
            else
            {
                return entry.CustomData;
            }
        }

        private void StoreData(TOCEntry entry, byte[] data)
        {
            entry.SetCustomData(data);
        }

        private void BuildFS()
        {
            RootDirectory = new Directory();
            RootDirectory.Name = "/";

            int entryCount = _imgFile.Header.EntryCount;
            for (int i = 0; i < entryCount; i++)
            {
                TOCEntry entry = _imgFile.TOC[i];
                Common.File.DataLoadDelegate load = () => LoadData(entry);
                Common.File.DataStoreDelegate store = data => StoreData(entry, data);
                Common.File.DataIsCustomDelegate isCustom = () => entry.CustomData != null;

                var file = new Common.File(load, store, isCustom)
                               {
                                   CompressedSize = entry.Size,
                                   IsCompressed = false,
                                   Name = _imgFile.TOC.GetName(i),
                                   Size = entry.Size,
                                   IsResource = entry.IsResourceFile,
                                   ResourceType = entry.ResourceType,
                                   ParentDirectory = RootDirectory
                               };

                RootDirectory.AddObject(file);
            }
        }
    }
}