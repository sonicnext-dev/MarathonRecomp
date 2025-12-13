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
using RageLib.Common;
using RageLib.Common.Resources;
using RageLib.Common.ResourceTypes;

namespace RageLib.Models.Resource.Models
{
    internal class Geometry : DATBase, IFileAccess
    {
        private uint Unknown1 { get; set; }
        private uint Unknown2 { get; set; }
        private uint Unknown3 { get; set; }
        private uint Unknown4 { get; set; }
        private uint Unknown5 { get; set; }
        private uint Unknown6 { get; set; }
        private uint Unknown7 { get; set; }
        private uint Unknown8 { get; set; }
        public uint IndexCount { get; private set; }
        public uint FaceCount { get; private set; }
        public ushort VertexCount { get; private set; }
        public ushort PrimitiveType { get; private set; }	// RAGE_PRIMITIVE_TYPE
        private uint Unknown9 { get; set; }
        public ushort VertexStride { get; private set; }
        private ushort Unknown10 { get; set; }
        private uint Unknown11 { get; set; }
        private uint Unknown12 { get; set; }
        private uint Unknown13 { get; set; }

        public VertexBuffer VertexBuffer { get; set; }
        public IndexBuffer IndexBuffer { get; set; }

        #region Implementation of IFileAccess

        public new void Read(BinaryReader br)
        {
            base.Read(br);

            Unknown1 = br.ReadUInt32();
            Unknown2 = br.ReadUInt32();

            var vertexBuffersOffset = ResourceUtil.ReadOffset(br);
            Unknown3 = br.ReadUInt32();
            Unknown4 = br.ReadUInt32();
            Unknown5 = br.ReadUInt32();

            var indexBuffersOffset = ResourceUtil.ReadOffset(br);
            Unknown6 = br.ReadUInt32();
            Unknown7 = br.ReadUInt32();
            Unknown8 = br.ReadUInt32();

            IndexCount = br.ReadUInt32();
            FaceCount = br.ReadUInt32();
            VertexCount = br.ReadUInt16();
            PrimitiveType = br.ReadUInt16();

            Unknown9 = br.ReadUInt32();

            VertexStride = br.ReadUInt16();
            Unknown10 = br.ReadUInt16();

            Unknown11 = br.ReadUInt32();
            Unknown12 = br.ReadUInt32();
            Unknown13 = br.ReadUInt32();

            // Data

            br.BaseStream.Seek(vertexBuffersOffset, SeekOrigin.Begin);
            VertexBuffer = new VertexBuffer(br);

            br.BaseStream.Seek(indexBuffersOffset, SeekOrigin.Begin);
            IndexBuffer = new IndexBuffer(br);
        }

        public new void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}