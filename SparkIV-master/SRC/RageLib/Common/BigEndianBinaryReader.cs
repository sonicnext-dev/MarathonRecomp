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

using System;
using System.IO;

namespace RageLib.Common
{
    public class BigEndianBinaryReader : BinaryReader
    {
        public BigEndianBinaryReader(Stream input) : base(input) { }

        public override short ReadInt16()
        {
            byte[] byteBuffer = base.ReadBytes(2);
            return (short)((byteBuffer[0] << 8) | byteBuffer[1]);
        }

        public override int ReadInt32()
        {
            byte[] byteBuffer = base.ReadBytes(4);
            return (int)((byteBuffer[0] << 24) | (byteBuffer[1] << 16) | (byteBuffer[2] << 8) | byteBuffer[3]);
        }

        public override ushort ReadUInt16()
        {
            byte[] byteBuffer = base.ReadBytes(2);
            return (ushort)((byteBuffer[0] << 8) | byteBuffer[1]);
        }

        public override uint ReadUInt32()
        {
            byte[] byteBuffer = base.ReadBytes(4);
            return (uint)((byteBuffer[0] << 24) | (byteBuffer[1] << 16) | (byteBuffer[2] << 8) | byteBuffer[3]);
        }

        public override float ReadSingle()
        {
            byte[] byteBuffer = BitConverter.GetBytes(ReadUInt32());
            return BitConverter.ToSingle(byteBuffer, 0);
        }
    }
}
