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

using System.IO;
using RageLib.Common.Resources;

namespace RageLib.Common.ResourceTypes
{
    public class PtrString : IFileAccess
    {
        private uint _offset;
        private string _value;

        public PtrString()
        {
        }

        public PtrString(BinaryReader br)
        {
            Read(br);
        }

        public uint Offset
        {
            get { return _offset; }
        }

        public string Value
        {
            get { return _value; }
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            _offset = ResourceUtil.ReadOffset(br);

            using (new StreamContext(br))
            {
                br.BaseStream.Seek(_offset, SeekOrigin.Begin);
                _value = ResourceUtil.ReadNullTerminatedString(br);
            }
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion

        #region Overrides of Object

        public override string ToString()
        {
            return Value;
        }

        #endregion
    }
}