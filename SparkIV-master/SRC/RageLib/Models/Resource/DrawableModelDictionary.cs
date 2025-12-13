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
using RageLib.Common.ResourceTypes;

namespace RageLib.Models.Resource
{
    class DrawableModelDictionary : PGDictionary<DrawableModel>, IDataReader, IEmbeddedResourceReader, IDisposable
    {
        public void ReadData(BinaryReader br)
        {
            foreach (var entry in Entries)
            {
                entry.ReadData(br);
            }
        }

        public void ReadEmbeddedResources(Stream systemMemory, Stream graphicsMemory)
        {
            foreach (var entry in Entries)
            {
                entry.ReadEmbeddedResources(systemMemory, graphicsMemory);
            }
        }

        #region Implementation of IDisposable

        public void Dispose()
        {
            foreach (var entry in Entries)
            {
                entry.Dispose();
            }
        }

        #endregion
    }
}
