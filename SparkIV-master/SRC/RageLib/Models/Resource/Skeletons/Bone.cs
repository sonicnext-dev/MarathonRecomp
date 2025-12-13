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

using System.Diagnostics;
using System.IO;
using RageLib.Common;
using RageLib.Common.Resources;
using RageLib.Common.ResourceTypes;

namespace RageLib.Models.Resource.Skeletons
{
    internal class Bone : IFileAccess
    {
        public Bone Parent { get; set; }
        public Bone NextSibling { get; set; }
        public Bone FirstChild { get; set; }

        public long Offset { get; private set; }

        public string Name { get; private set; }
        private short Unknown1;
        private short Unknown2;

        public uint NextSiblingOffset { get; private set; }
        public uint FirstChildOffset { get; private set; }
        public uint ParentOffset { get; private set; }

        public short BoneIndex { get; private set; }
        public short BoneID { get; private set; }
        private short BoneIndex2;
        private short Unknown3;
        private int Unknown4;

        public Vector4 Position { get; private set; }
        public Vector4 RotationEuler { get; private set; }
        public Vector4 RotationQuaternion { get; private set; }
        
        private Vector4 UnknownZeroVector1;

        public Vector4 AbsolutePosition { get; private set; }
        public Vector4 AbsoluteRotationEuler { get; private set; }
        private Vector4 UnknownZeroVector2;
        private Vector4 UnknownZeroVector3;
        private Vector4 UnknownZeroVector4;
        private Vector4 MinPI;
        private Vector4 MaxPI;
        private Vector4 UnknownAllZeros;

        public Bone()
        {
        }

        public Bone(BinaryReader br)
        {
            Read(br);
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            Offset = br.BaseStream.Position;

            Name = new PtrString(br).Value;
            
            Unknown1 = br.ReadInt16();
            Unknown2 = br.ReadInt16();

            NextSiblingOffset = ResourceUtil.ReadOffset(br);
            FirstChildOffset = ResourceUtil.ReadOffset(br);
            ParentOffset = ResourceUtil.ReadOffset(br);

            BoneIndex = br.ReadInt16();
            BoneID = br.ReadInt16();
            BoneIndex2 = br.ReadInt16();
            Unknown3 = br.ReadInt16();

            //Debug.Assert(BoneIndex == BoneIndex2);

            Unknown4 = br.ReadInt32();

            Position = new Vector4(br);
            RotationEuler = new Vector4(br);
            RotationQuaternion = new Vector4(br);

            UnknownZeroVector1 = new Vector4(br);

            AbsolutePosition = new Vector4(br);
            AbsoluteRotationEuler = new Vector4(br);

            UnknownZeroVector2 = new Vector4(br);
            UnknownZeroVector3 = new Vector4(br);
            UnknownZeroVector4 = new Vector4(br);

            MinPI = new Vector4(br); // Minimum euler rotation maybe?
            MaxPI = new Vector4(br); // Maximum euler rotation maybe?

            UnknownAllZeros = new Vector4(br);

        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion

        #region Overrides of Object

        public override string ToString()
        {
            return Name;
        }

        #endregion
    }
}