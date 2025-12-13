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

namespace RageLib.FileSystem.Common
{
    public class Directory : FSObject, IEnumerable<FSObject>
    {
        private readonly List<FSObject> _fsObjects = new List<FSObject>();
        private readonly Dictionary<string, FSObject> _fsObjectsByName = new Dictionary<string, FSObject>();

        public override bool IsDirectory
        {
            get { return true; }
        }

        public FSObject this[int index]
        {
            get { return _fsObjects[index]; }
        }

        public FSObject FindByName(string name)
        {
            FSObject obj;
            _fsObjectsByName.TryGetValue(name.ToLower(), out obj);
            return obj;
        }

        #region IEnumerable<FSObject> Members

        public IEnumerator<FSObject> GetEnumerator()
        {
            return _fsObjects.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return _fsObjects.GetEnumerator();
        }

        #endregion

        public void AddObject(FSObject obj)
        {
            _fsObjects.Add(obj);
            _fsObjectsByName.Add(obj.Name.ToLower(), obj);
        }

        public void DeleteObject(FSObject obj)
        {
            _fsObjectsByName.Remove(obj.Name.ToLower());
            _fsObjects.Remove(obj);
        }
    }
}