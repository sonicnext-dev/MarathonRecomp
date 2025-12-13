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
    public class KeyUtilEFLC : KeyUtil
    {
        public override string ExecutableName
        {
            get { return "EFLC.exe"; }
        }

        protected override string[] PathRegistryKeys
        {
            get
            {
                return new[]
                           {
                               @"SOFTWARE\Rockstar Games\EFLC",
                               @"SOFTWARE\Wow6432Node\Rockstar Games\EFLC"
                           };
            }
        }

        protected override uint[] SearchOffsets
        {
            get
            {
                return new uint[]
                           {
                               //EFLC
                               0xB82A28 /* 1.1.3 */,
                               0xBEF028 /* 1.1.2 */,
                               0xC705E0 /* 1.1.1 */,
                               0xC6DEEC /* 1.1.0 */,
                           };
            }
        }
    }
}