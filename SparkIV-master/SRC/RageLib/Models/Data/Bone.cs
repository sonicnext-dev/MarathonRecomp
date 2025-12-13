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
using RageLib.Common.ResourceTypes;

namespace RageLib.Models.Data
{
    public class Bone
    {
        public Bone Parent { get; private set; }
        public string Name { get; private set; }
        public int Index { get; private set; }
        
        public Vector4 Position { get; private set; }
        public Vector4 Rotation { get; private set; }

        public Vector4 AbsolutePosition { get; private set; }
        public Vector4 AbsoluteRotation { get; private set; }

        public List<Bone> Children { get; private set; }

        internal Bone(Resource.Skeletons.Bone bone, Bone parent)
        {
            Parent = parent;
            Name = bone.Name;
            Index = bone.BoneIndex;

            Position = bone.Position;
            Rotation = bone.RotationEuler;

            AbsolutePosition = bone.AbsolutePosition;
            AbsoluteRotation = bone.AbsoluteRotationEuler;

            Children = new List<Bone>();
        }

        #region Overrides of Object

        public override string ToString()
        {
            return Name;
        }

        #endregion
    }
}
