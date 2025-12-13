/**********************************************************************\

 RageLib
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

namespace RageLib.Common.ResourceTypes
{
    public struct Vector3 : IFileAccess
    {
        public float X { get; set; }
        public float Y { get; set; }
        public float Z { get; set; }

        public Vector3(float x, float y, float z) : this()
        {
            X = x;
            Y = y;
            Z = z;
        }

        public Vector3(BinaryReader br) : this()
        {
            Read(br);
        }

        public void Read(BinaryReader br)
        {
            X = br.ReadSingle();
            Y = br.ReadSingle();
            Z = br.ReadSingle();
        }

        public void Write(BinaryWriter bw)
        {
            throw new NotImplementedException();
        }
    }
}