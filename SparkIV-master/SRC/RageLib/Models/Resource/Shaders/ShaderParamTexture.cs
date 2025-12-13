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
using RageLib.Common.Resources;
using RageLib.Common.ResourceTypes;

namespace RageLib.Models.Resource.Shaders
{
    internal class ShaderParamTexture : DATBase, IShaderParam
    {
        private uint Unknown1 { get; set; }
        private ushort Unknown2 { get; set; }
        private ushort Unknown3 { get; set; }
        private uint Unknown4 { get; set; }
        private uint Unknown5 { get; set; }
        private uint TextureNameOffset { get; set; }
        private uint Unknown7 { get; set; }

        public string TextureName { get; private set; }

        #region Implementation of IFileAccess

        public new void Read(BinaryReader br)
        {
            base.Read(br);
            
            Unknown1 = br.ReadUInt32();
            Unknown2 = br.ReadUInt16();
            Unknown3 = br.ReadUInt16();
            Unknown4 = br.ReadUInt32();
            Unknown5 = br.ReadUInt32();
            TextureNameOffset = ResourceUtil.ReadOffset(br);
            Unknown7 = br.ReadUInt32();

            br.BaseStream.Seek(TextureNameOffset, SeekOrigin.Begin);
            TextureName = ResourceUtil.ReadNullTerminatedString(br);
        }

        public new void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}