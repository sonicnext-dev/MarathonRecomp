/**********************************************************************\

 RageLib - HyperText
 Copyright (C) 2008-2009  Arushan/Aru <oneforaru at gmail.com>

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
using RageLib.Common.Resources;
using RageLib.Common.ResourceTypes;
using RageLib.Textures;

namespace RageLib.HyperText.Resource
{
    class HtmlDocument : IFileAccess
    {
        public HtmlNode RootElement { get; private set; }

        private uint BodyOffset { get; set; }
        private uint Unknown1Offset { get; set; }
        
        public uint TextureDictionaryOffset { get; set; }
        public TextureFile TextureDictionary { get; set; }

        private PtrCollection<UnDocData> Unknown2 { get; set; }
        private PtrCollection<HtmlNode> ChildNodes { get; set; }
        private PtrCollection<UnDocData> Unknown3 { get; set; }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            RootElement = new PtrValue<HtmlNode>(br).Value;

            BodyOffset = ResourceUtil.ReadOffset(br);
            Unknown1Offset = ResourceUtil.ReadOffset(br);
            TextureDictionaryOffset = ResourceUtil.ReadOffset(br);

            Unknown2 = new PtrCollection<UnDocData>(br);
            ChildNodes = new PtrCollection<HtmlNode>(br);
            Unknown3 = new PtrCollection<UnDocData>(br);
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion

        public void ReadEmbeddedResources(Stream systemMemory, Stream graphicsMemory)
        {
            if (TextureDictionaryOffset != 0)
            {
                systemMemory.Seek(TextureDictionaryOffset, SeekOrigin.Begin);

                TextureDictionary = new TextureFile();
                TextureDictionary.Open(systemMemory, graphicsMemory);
            }
        }

        public void ReadData(BinaryReader br)
        {
            
        }
    }
}
