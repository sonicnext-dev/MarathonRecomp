/**********************************************************************\

 RageLib - Models
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

using System.Collections.Generic;
using System.IO;
using RageLib.Common;

namespace RageLib.Models.Resource.Models
{
    internal class VertexDeclaration : IFileAccess
    {
        public uint UsageFlags { get; set; }
        public ushort Stride { get; set; }
        public byte AlterateDecoder { get; set; }
        public byte Type { get; set; }
        public ulong DeclarationTypes { get; set; }

        public VertexDeclaration(BinaryReader br)
        {
            Read(br);
        }

        private static VertexElementType GetType(ulong typeDecl, int index)
        {
            return (VertexElementType)((typeDecl >> (4 * index)) & 0xF);
        }

        private static int GetSize(VertexElementType type)
        {
            int[] sizeMapping = {2, 4, 6, 8, 4, 8, 12, 16, 4, 4, 4, 0, 0, 0, 0, 0};
            return sizeMapping[(int) type];
        }

        private void DecodeSingleElement(ICollection<VertexElement> list, int index, int streamIndex, VertexElementUsage usage, int usageIndex)
        {
            DecodeSingleElement(list, index, streamIndex, usage, ref usageIndex);
        }

        private void DecodeSingleElement(ICollection<VertexElement> list, int index, int streamIndex, VertexElementUsage usage, ref int usageIndex)
        {
            var declTypes = DeclarationTypes;
            var usageFlags = UsageFlags;
            var usageFlagMask = (uint)(1 << index);
            var type = GetType(declTypes, index);
            var size = GetSize(type);

            if ((usageFlags & usageFlagMask) != 0)
            {
                var element = new VertexElement()
                                  {
                                      UsageIndex = usageIndex++,
                                      StreamIndex = streamIndex,
                                      Usage = usage,
                                      Type = type,
                                      Size = size,
                                  };
                list.Add(element);
            }
        }

        public VertexElement[] DecodeAsVertexElements()
        {
            var elements = new List<VertexElement>();
            int streamIndex = 0;
            int usageIndexPosition = 0;
            int usageIndexBlendWeight = 0;
            int usageIndexBlendIndices = 0;
            int usageIndexNormal = 0;
            int usageIndexTexture = 0;
            int usageIndexTangent = 0;
            int usageIndexBinormal = 0;

            DecodeSingleElement(elements, 0, streamIndex, VertexElementUsage.Position, ref usageIndexPosition);
            DecodeSingleElement(elements, 1, streamIndex, VertexElementUsage.BlendWeight, ref usageIndexBlendWeight);
            DecodeSingleElement(elements, 2, streamIndex, VertexElementUsage.BlendIndices, ref usageIndexBlendIndices);
            DecodeSingleElement(elements, 3, streamIndex, VertexElementUsage.Normal, ref usageIndexNormal);
            DecodeSingleElement(elements, 4, streamIndex, VertexElementUsage.Color, 0); // Diffuse?
            DecodeSingleElement(elements, 5, streamIndex, VertexElementUsage.Color, 1); // Specular?
            for(int i = 6; i<14; i++) // 8
            {
                DecodeSingleElement(elements, i, streamIndex, VertexElementUsage.TextureCoordinate, ref usageIndexTexture);
            }
            DecodeSingleElement(elements, 14, streamIndex, VertexElementUsage.Tangent, ref usageIndexTangent);
            DecodeSingleElement(elements, 15, streamIndex, VertexElementUsage.Binormal, ref usageIndexBinormal);

            return elements.ToArray();
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            UsageFlags = br.ReadUInt32();
            Stride = br.ReadUInt16();
            AlterateDecoder = br.ReadByte();
            Type = br.ReadByte();

            DeclarationTypes = br.ReadUInt64();
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}