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

using RageLib.Common.Resources;

namespace RageLib.FileSystem.Common
{
    public class File : FSObject
    {
        #region Delegates

        public delegate byte[] DataLoadDelegate();
        public delegate void DataStoreDelegate(byte[] data);
        public delegate bool DataIsCustomDelegate();

        #endregion

        private readonly DataLoadDelegate _dataLoad;
        private readonly DataStoreDelegate _dataStore;
        private readonly DataIsCustomDelegate _dataCustom;

        public File(DataLoadDelegate dataLoad)
        {
            _dataLoad = dataLoad;
            _dataStore = delegate { };
            _dataCustom = (() => false);
        }

        public File(DataLoadDelegate dataLoad, DataStoreDelegate dataStore, DataIsCustomDelegate dataCustom)
        {
            _dataLoad = dataLoad;
            _dataStore = dataStore;
            _dataCustom = dataCustom;
        }

        public override bool IsDirectory
        {
            get { return false; }
        }

        public bool IsCompressed { get; set; }
        public int CompressedSize { get; set; }

        public int Size { get; set; }

        public bool IsResource { get; set; }
        public ResourceType ResourceType { get; set; }

        public bool IsCustomData
        {
            get { return _dataCustom(); }
        }

        public byte[] GetData()
        {
            return _dataLoad();
        }

        public void SetData(byte[] data)
        {
            _dataStore(data);                
        }
    }
}