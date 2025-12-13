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

using System.Collections.Generic;
using System.IO;
using RageLib.Common.Resources;

namespace RageLib.FileSystem.Real
{
    class FileEntry : RealEntry
    {
        private readonly FileInfo _file;

        public ResourceType ResourceType { get; private set; }

        public bool IsResourceFile { get; private set; }

        private List<string> _resourceFiles = new List<string>()
                                                  {
                                                      ".wtd", ".wdr", ".wdd", ".wft",
                                                      ".wpfl", ".whm", ".wad", ".wbd", 
                                                      ".wbn", ".wbs"
                                                  };


        public FileEntry(RealContext context, FileInfo file)
        {
            Context = context;
            _file = file;

            string ext = file.Extension;

            if (_resourceFiles.Contains(ext))
            {

                FileStream fs = _file.OpenRead();
                try
                {
                    IsResourceFile = ResourceUtil.IsResource(fs);

                    if (IsResourceFile)
                    {
                        fs.Position = 0;
                        ResourceType resType;
                        uint flags;
                        ResourceUtil.GetResourceData(fs, out flags, out resType);
                        ResourceType = resType;
                    }
                }
                catch
                {
                    ResourceType = 0;
                    IsResourceFile = false;
                }
                finally
                {
                    fs.Close();
                }

            }
        }

        public override bool IsDirectory
        {
            get { return false; }
        }

        public override string Name
        {
            get { return _file.Name; }
        }

        public string FullName
        {
            get { return _file.FullName; }
        }

        public int Size
        {
            get { return (int)_file.Length; }
        }

        public byte[] GetData()
        {
            return File.ReadAllBytes(_file.FullName);
        }

        public void SetData(byte[] data)
        {
            File.WriteAllBytes(_file.FullName, data);
        }
    }
}