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

namespace RageLib.Models.Data
{
    public class Skeleton
    {
        public Bone RootBone { get; private set; }

        private Dictionary<int, Bone> _bonesByIndex;

        internal Skeleton(Resource.Skeletons.Skeleton skeleton)
        {
            _bonesByIndex = new Dictionary<int, Bone>();
            RootBone = BuildBone(skeleton.Bones[0], null);
        }

        public Bone this[int index]
        {
            get { return _bonesByIndex[index]; }
        }

        private Bone BuildBone(Resource.Skeletons.Bone bone, Bone parent)
        {
            var dataBone = new Bone(bone, parent);
            _bonesByIndex.Add(dataBone.Index, dataBone);

            var childBone = bone.FirstChild;
            while(childBone != null)
            {
                dataBone.Children.Add( BuildBone(childBone, dataBone) );
                childBone = childBone.NextSibling;
            }

            return dataBone;
        }
    }
}
