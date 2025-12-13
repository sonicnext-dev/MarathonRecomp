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

using System.Collections;
using System.Collections.Generic;
using System.IO;
using RageLib.Common.Resources;

namespace RageLib.Common.ResourceTypes
{
    public class PtrCollection<T> : IFileAccess, IEnumerable<T> where T : class, IFileAccess, new()
    {
        private uint[] _itemOffsets;
        private List<T> _items;

        public ushort Count { get; set; }
        public ushort Size { get; set; }

        public PtrCollection()
        {
            
        }

        public PtrCollection(BinaryReader br)
        {
            Read(br);
        }

        public T GetByOffset(uint offset)
        {
            for(int i=0; i<_itemOffsets.Length; i++)
            {
                if (_itemOffsets[i] == offset)
                {
                    return _items[i];
                }
            }
            return null;
        }

        public T this[int index]
        {
            get { return _items[index]; }
            set { _items[index] = value;  }
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            var ptrListOffset = ResourceUtil.ReadOffset(br);
            Count = br.ReadUInt16();
            Size = br.ReadUInt16();

            _itemOffsets = new uint[Count];
            _items = new List<T>();

            using (new StreamContext(br))
            {
                br.BaseStream.Seek(ptrListOffset, SeekOrigin.Begin);

                for (int i = 0; i < Count; i++)
                {
                    _itemOffsets[i] = ResourceUtil.ReadOffset(br);
                }

                for (int i = 0; i < Count; i++)
                {
                    br.BaseStream.Seek(_itemOffsets[i], SeekOrigin.Begin);
                    var item = new T();
                    item.Read(br);
                    _items.Add(item);
                }
            }
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion

        #region Implementation of IEnumerable<T>

        public IEnumerator<T> GetEnumerator()
        {
            return _items.GetEnumerator();
        }
        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        #endregion
    }
}
