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
using System.IO;
using RageLib.Common;
using RageLib.Common.Resources;
using RageLib.Common.ResourceTypes;
using RageLib.Textures;

namespace RageLib.Models.Resource.Shaders
{
    /// grmShaderGroup 
    class ShaderGroup : DATBase, IFileAccess
    {
        public uint TextureDictionaryOffset { get; private set; }
        public TextureFile TextureDictionary { get; set; }

        public PtrCollection<ShaderFx> Shaders { get; private set; }

        private SimpleArray<uint> Zeros { get; set; }

        private SimpleCollection<uint> VertexDeclarationUsageFlags { get; set; }

        private SimpleCollection<uint> Data3 { get; set; }

        public ShaderGroup()
        {
        }

        public ShaderGroup(BinaryReader br)
        {
            Read(br);
        }

        #region Implementation of IFileAccess

        public new void Read(BinaryReader br)
        {
            base.Read(br);

            TextureDictionaryOffset = ResourceUtil.ReadOffset(br);

            // CPtrCollection<T>
            Shaders = new PtrCollection<ShaderFx>(br);

            Zeros = new SimpleArray<uint>(br, 12, r => r.ReadUInt32());

            VertexDeclarationUsageFlags = new SimpleCollection<uint>(br, reader => reader.ReadUInt32());

            Data3 = new SimpleCollection<uint>(br, reader => reader.ReadUInt32());
        }

        public new void Write(BinaryWriter bw)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}