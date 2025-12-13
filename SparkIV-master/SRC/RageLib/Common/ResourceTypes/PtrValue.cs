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
    public class PtrValue<T> : IFileAccess where T : class, IFileAccess, new()
    {
        private uint _offset;
        private T _value;

        public PtrValue()
        {
        }

        public PtrValue(BinaryReader br)
        {
            Read(br);
        }

        public uint Offset
        {
            get { return _offset; }
        }

        public T Value
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

                _value = new T();
                _value.Read(br);                
            }
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}
