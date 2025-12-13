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
using System.Data;
using System.IO;
using RageLib.Common;
using RageLib.Common.Resources;

namespace RageLib.HyperText.Resource
{
    class File
    {
        public HtmlDocument Data { get; private set; }

        public void Open(string filename)
        {
            var fs = new FileStream(filename, FileMode.Open, FileAccess.ReadWrite);
            try
            {
                Open(fs);
            }
            finally
            {
                fs.Close();
            }
        }

        public void Open(Stream stream)
        {
            var res = new ResourceFile();
            res.Read(stream);

            if (res.Type != ResourceType.Generic)
            {
                throw new Exception("Not a supported file type.");
            }

            var systemMemory = new MemoryStream(res.SystemMemData);
            var graphicsMemory = new MemoryStream(res.GraphicsMemData);

            Data = new HtmlDocument();

            // Read System Memory
            
            var systemMemoryBR = new BinaryReader(systemMemory);

            Data.Read(systemMemoryBR);

            // Read Graphics Memory

            var graphicsMemoryBR = new BinaryReader(graphicsMemory);

            Data.ReadData(graphicsMemoryBR);

            // Read Embedded Resource Files

            Data.ReadEmbeddedResources(systemMemory, graphicsMemory);

            systemMemory.Close();
            graphicsMemory.Close();

        }
    }
}