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
using RageLib.Common.Resources;

namespace RageLib.Textures.Resource
{
    internal class File : IDisposable
    {
        private ResourceFile _resourceFile;

        public Header Header { get; private set; }

        public Dictionary<uint, TextureInfo> TexturesByHash { get; private set; }
        public List<TextureInfo> Textures { get; private set; }

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

        public void Open(Stream systemMemory, Stream graphicsMemory)
        {
            // Sys

            var ms = systemMemory;
            var br = new BinaryReader(ms);

            Header = new Header();
            Header.Read(br);

            TexturesByHash = new Dictionary<uint, TextureInfo>(Header.TextureCount);
            Textures = new List<TextureInfo>(Header.TextureCount);

            var textureHashes = new uint[Header.TextureCount];
            var infoOffsets = new uint[Header.TextureCount];

            ms.Seek(Header.HashTableOffset, SeekOrigin.Begin);
            for (int i = 0; i < Header.TextureCount; i++)
            {
                textureHashes[i] = br.ReadUInt32();
            }

            ms.Seek(Header.TextureListOffset, SeekOrigin.Begin);
            for (int i = 0; i < Header.TextureCount; i++)
            {
                infoOffsets[i] = ResourceUtil.ReadOffset(br);
            }

            for (int i = 0; i < Header.TextureCount; i++)
            {
                ms.Seek(infoOffsets[i], SeekOrigin.Begin);

                var info = new TextureInfo { File = this };
                info.Read(br);

                Textures.Add(info);
                TexturesByHash.Add(textureHashes[i], info);
            }

            // Gfx

            ms = graphicsMemory;
            br = new BinaryReader(ms);

            for (int i = 0; i < Header.TextureCount; i++)
            {
                Textures[i].ReadData(br);
            }
        }

        public void Open(Stream stream)
        {
            var res = new ResourceFile();
            res.Read(stream);

            if (res.Type != ResourceType.Texture)
            {
                throw new Exception("Not a valid texture resource.");
            }

            // Read

            var systemMem = new MemoryStream(res.SystemMemData);
            var graphicsMem = new MemoryStream(res.GraphicsMemData);

            Open(systemMem, graphicsMem);

            systemMem.Close();
            graphicsMem.Close();

            // Save the resource file for later
            _resourceFile = res;
        }

        public void Save(Stream stream)
        {
            var res = _resourceFile;

            // Save to the Resource file stream

            var systemMem = new MemoryStream(res.SystemMemData);
            var graphicsMem = new MemoryStream(res.GraphicsMemData);

            Save(systemMem, graphicsMem);

            systemMem.Close();
            graphicsMem.Close();

            // Now write the Resource data back to the stream

            res.Write(stream);
        }

        public void Save(Stream systemMemory, Stream graphicsMemory)
        {
            var ms = graphicsMemory;
            var bw = new BinaryWriter(ms);

            for (int i = 0; i < Header.TextureCount; i++)
            {
                Textures[i].WriteData(bw);
            }           
        }

        #region Implementation of IDisposable

        public void Dispose()
        {
            if (_resourceFile != null)
            {
                _resourceFile.Dispose();
            }

            if (TexturesByHash != null)
            {
                TexturesByHash.Clear();
            }

            if (Textures != null)
            {
                Textures.Clear();
            }
        }

        #endregion
    }
}