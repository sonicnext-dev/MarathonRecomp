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

namespace RageLib.Common.ResourceTypes
{
    public class SimpleArray<T> : IFileAccess, IEnumerable<T>
    {
        public delegate T ReadDataDelegate(BinaryReader br);

        protected ReadDataDelegate ReadData;

        protected List<T> Values;
        public int Count { get; private set; }

        public SimpleArray(int count, ReadDataDelegate delg)
        {
            Count = count;
            ReadData = delg;
        }

        public SimpleArray(BinaryReader br, int count, ReadDataDelegate delg)
            : this(count, delg)
        {
            Read(br);
        }

        public T this[int index]
        {
            get { return Values[index];  }
            set { Values[index] = value; }
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            Values = new List<T>(Count);

            for (int i = 0; i < Count; i++)
            {
                Values.Add( ReadData(br) );
            }
        }

        public void Write(BinaryWriter bw)
        {
            
        }

        #endregion

        #region Implementation of IEnumerable

        public IEnumerator<T> GetEnumerator()
        {
            return Values.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        #endregion
    }
}