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

using System.Collections.Generic;
using System.IO;
using RageLib.Common;
using RageLib.Common.Resources;
using RageLib.Common.ResourceTypes;

namespace RageLib.Models.Resource.Skeletons
{
    // rageSkeleton (real name unknown)
    class Skeleton : IFileAccess
    {
        class SubStruct : DATBase, IFileAccess
        {
            private int Unknown1 { get; set; }
            private int Unknown2 { get; set; }

            public SubStruct(BinaryReader br)
            {
                Read(br);
            }

            #region Implementation of IFileAccess

            public new void Read(BinaryReader br)
            {
                base.Read(br);
                Unknown1 = br.ReadInt32();
                Unknown2 = br.ReadInt32();
            }

            public new void Write(BinaryWriter bw)
            {
                throw new System.NotImplementedException();
            }

            #endregion
        }

        public ushort BoneCount { get; private set; }
        private short Unknown0;
        private int Unknown1;
        private int Unknown2;

        public SimpleCollection<BoneIDMapping> BoneIDMappings { get; private set; }

        private int Unknown3;
        private uint UnknownHash;
        private int Unknown4;
        
        private SubStruct UnknownSubStruct;

        public Skeleton()
        {
        }

        public Skeleton(BinaryReader br)
        {
            Read(br);
        }

        public SimpleArray<Bone> Bones { get; private set; }
        public SimpleArray<int> UnknownInts { get; private set; }
        public SimpleArray<Matrix44> Transforms1 { get; private set; }
        public SimpleArray<Matrix44> Transforms2 { get; private set; }
        public SimpleArray<Matrix44> Transforms3 { get; private set; }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            uint bonesOffset = ResourceUtil.ReadOffset(br);
            uint unknownIntsOffset = ResourceUtil.ReadOffset(br);
            uint transforms1Offset = ResourceUtil.ReadOffset(br);
            uint transforms2Offset = ResourceUtil.ReadOffset(br);
            uint transforms3Offset = ResourceUtil.ReadOffset(br);

            BoneCount = br.ReadUInt16();
            Unknown0 = br.ReadInt16();
            Unknown1 = br.ReadInt32();
            Unknown2 = br.ReadInt32();

            BoneIDMappings = new SimpleCollection<BoneIDMapping>(br, r => new BoneIDMapping(r));

            Unknown3 = br.ReadInt32();
            UnknownHash = br.ReadUInt32();
            Unknown4 = br.ReadInt32();

            UnknownSubStruct = new SubStruct(br);

            // Data:

            br.BaseStream.Seek(bonesOffset, SeekOrigin.Begin);
            Bones = new SimpleArray<Bone>(br, BoneCount, r => new Bone(r));

            br.BaseStream.Seek(unknownIntsOffset, SeekOrigin.Begin);
            UnknownInts = new SimpleArray<int>(br, BoneCount, r => r.ReadInt32());

            br.BaseStream.Seek(transforms1Offset, SeekOrigin.Begin);
            Transforms1 = new SimpleArray<Matrix44>(br, BoneCount, r => new Matrix44(r));

            br.BaseStream.Seek(transforms2Offset, SeekOrigin.Begin);
            Transforms2 = new SimpleArray<Matrix44>(br, BoneCount, r => new Matrix44(r));

            br.BaseStream.Seek(transforms3Offset, SeekOrigin.Begin);
            Transforms3 = new SimpleArray<Matrix44>(br, BoneCount, r => new Matrix44(r));

            // Fun stuff...
            // Build a mapping of Offset -> Bone
            var boneOffsetMapping = new Dictionary<uint, Bone>();
            boneOffsetMapping.Add(0, null);
            foreach (var bone in Bones)
            {
                boneOffsetMapping.Add((uint)bone.Offset, bone);
            }

            // Now resolve all the bone offsets to the real bones
            foreach (var bone in Bones)
            {
                bone.Parent = boneOffsetMapping[bone.ParentOffset];
                bone.FirstChild = boneOffsetMapping[bone.FirstChildOffset];
                bone.NextSibling = boneOffsetMapping[bone.NextSiblingOffset];
            }
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}
