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

namespace RageLib.Common
{
    public static class KeyStore
    {
        public delegate byte[] KeyLoader();

        private static byte[] _aesKey;
        private static KeyLoader _keyLoader;

        static KeyStore()
        {
            // Default Key Loader
            SetKeyLoader( () =>
                              {
                                  var util = new KeyUtilGTAIV();
                                  return util.FindKey(util.FindGameDirectory());
                              } );
        }

        public static void SetKeyLoader(KeyLoader loader)
        {
            _keyLoader = loader;
        }

        public static byte[] AESKey
        {
            get
            {
                if (_aesKey == null)
                {
                    _aesKey = _keyLoader();
                }
                return _aesKey;
            }
        }
    }
}