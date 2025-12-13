/**********************************************************************\

 RageLib - Models
 Copyright (C) 2009  Arushan/Aru <oneforaru at gmail.com>

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
using RageLib.Models.Resource;

namespace RageLib.Models.Data
{
    public class Mesh
    {
        public PrimitiveType PrimitiveType { get; private set; }
        public int FaceCount { get; private set; }

        public int VertexCount { get; private set; }
        public byte[] VertexData { get; private set; }
        public VertexDeclaration VertexDeclaration { get; private set; }
        public bool VertexHasNormal { get; set; }
        public bool VertexHasTexture { get; set; }
        public bool VertexHasBlendInfo { get; set; }
        public int VertexStride { get; private set; }
        
        public int IndexCount { get; private set; }
        public byte[] IndexData { get; private set; }

        public int MaterialIndex { get; set; }

        internal Mesh(Resource.Models.Geometry info)
        {
            PrimitiveType = (PrimitiveType) info.PrimitiveType;
            
            FaceCount = (int) info.FaceCount;
            
            VertexCount = info.VertexCount;
            VertexStride = info.VertexStride;
            VertexData = info.VertexBuffer.RawData;

            IndexCount = (int) info.IndexCount;
            IndexData = info.IndexBuffer.RawData;

            VertexDeclaration = new VertexDeclaration(info.VertexBuffer.VertexDeclaration);
            foreach (var element in VertexDeclaration.Elements)
            {
                if (element.Usage == VertexElementUsage.Normal)
                {
                    VertexHasNormal = true;
                }
                if (element.Usage == VertexElementUsage.TextureCoordinate)
                {
                    VertexHasTexture = true;
                }
                if (element.Usage == VertexElementUsage.BlendIndices)
                {
                    VertexHasBlendInfo = true;
                }
            }
        }

        public ushort[] DecodeIndexData()
        {
            byte[] indexData = IndexData;
            ushort[] indices = new ushort[IndexCount];
            for (int i = 0; i < IndexCount; i++)
            {
                indices[i] = BitConverter.ToUInt16(indexData, i*2);
            }
            return indices;
        }

        public Vertex[] DecodeVertexData()
        {
            byte[] vertexData = VertexData;
            Vertex[] vertices = new Vertex[VertexCount];

            using(MemoryStream ms = new MemoryStream(vertexData))
            {
                BinaryReader br = new BinaryReader(ms);
                for (int i = 0; i < VertexCount; i++)
                {
                    ms.Seek(i*VertexStride, SeekOrigin.Begin);
                    vertices[i] = new Vertex(br, this);
                }
            }

            return vertices;
        }
    }
}