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
using RageLib.Common;
using RageLib.Common.Resources;
using RageLib.Common.ResourceTypes;

namespace RageLib.Models.Resource.Shaders
{
    internal class Shader : PGBase, IFileAccess
    {
        private ushort Unknown1 { get; set; }
        private byte Unknown2 { get; set; }
        private byte Unknown3 { get; set; }
        private ushort Unknown4 { get; set; }
        private ushort Unknown4_1 { get; set; }
        private uint Unknown5 { get; set; }
        private uint Unknown6 { get; set; }
        public int ShaderParamCount { get; set; }
        private uint Unknown8 { get; set; }
        public uint Hash { get; private set; }
        private uint Unknown9 { get; set; }
        private uint Unknown10 { get; set; }
        private uint Unknown11 { get; set; }
        private uint Unknown12 { get; set; }
        private uint Unknown13 { get; set; }

        private SimpleArray<uint> ShaderParamOffsets { get; set; }
        private SimpleArray<byte> ShaderParamTypes { get; set; }
        private SimpleArray<uint> ShaderParamNames { get; set; }

        public Dictionary<ParamNameHash, IShaderParam> ShaderParams { get; private set; }

        public T GetInfoData<T>(ParamNameHash hash) where T : class, IShaderParam
        {
            IShaderParam value;
            ShaderParams.TryGetValue(hash, out value);
            return value as T;
        }

        #region Implementation of IFileAccess

        public new void Read(BinaryReader br)
        {
            base.Read(br);

            Unknown1 = br.ReadUInt16();
            Unknown2 = br.ReadByte();
            Unknown3 = br.ReadByte();

            Unknown4 = br.ReadUInt16();
            Unknown4_1 = br.ReadUInt16();

            Unknown5 = br.ReadUInt32();

            var shaderParamOffsetsOffset = ResourceUtil.ReadOffset(br);

            Unknown6 = br.ReadUInt32();
            ShaderParamCount = br.ReadInt32();
            Unknown8 = br.ReadUInt32();

            var shaderParamTypesOffset = ResourceUtil.ReadOffset(br);

            Hash = br.ReadUInt32();
            Unknown9 = br.ReadUInt32();
            Unknown10 = br.ReadUInt32();

            var shaderParamNameOffset = ResourceUtil.ReadOffset(br);

            Unknown11 = br.ReadUInt32();
            Unknown12 = br.ReadUInt32();
            Unknown13 = br.ReadUInt32();

            // Data :

            using (new StreamContext(br))
            {
                br.BaseStream.Seek(shaderParamOffsetsOffset, SeekOrigin.Begin);
                ShaderParamOffsets = new SimpleArray<uint>(br, ShaderParamCount, ResourceUtil.ReadOffset);

                br.BaseStream.Seek(shaderParamTypesOffset, SeekOrigin.Begin);
                ShaderParamTypes = new SimpleArray<byte>(br, ShaderParamCount, r => r.ReadByte());

                br.BaseStream.Seek(shaderParamNameOffset, SeekOrigin.Begin);
                ShaderParamNames = new SimpleArray<uint>(br, ShaderParamCount, r => r.ReadUInt32());

                ShaderParams = new Dictionary<ParamNameHash, IShaderParam>(ShaderParamCount);
                for (int i = 0; i < ShaderParamCount; i++)
                {
                    try
                    {
                        var obj = ParamObjectFactory.Create((ParamType) ShaderParamTypes[i]);

                        br.BaseStream.Seek(ShaderParamOffsets[i], SeekOrigin.Begin);
                        obj.Read(br);

                        ShaderParams.Add((ParamNameHash) ShaderParamNames[i], obj);
                    }
                    catch
                    {
                        ShaderParams.Add((ParamNameHash) ShaderParamNames[i], null);
                    }
                }
            }
        }

        public new void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}