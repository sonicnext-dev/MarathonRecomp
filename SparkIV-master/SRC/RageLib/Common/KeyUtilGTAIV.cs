﻿
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
    public class KeyUtilGTAIV : KeyUtil
    {
        public override string ExecutableName
        {
            get { return "GTAIV.exe"; }
        }

        protected override string[] PathRegistryKeys
        {
            get
            {
                return new[]
                           {
                               @"SOFTWARE\Rockstar Games\Grand Theft Auto IV", // 32bit
                               @"SOFTWARE\Wow6432Node\Rockstar Games\Grand Theft Auto IV" // 64bit
                           };
            }
        }

        protected override uint[] SearchOffsets
        {
            get
            {
                return new uint[]
                           {
                               //EFIGS EXEs
                               0xA94204 /* 1.0 */,
                               0xB607C4 /* 1.0.1 */,
                               0xB56BC4 /* 1.0.2 */,
                               0xB75C9C /* 1.0.3 */,
                               0xB7AEF4 /* 1.0.4 */,
                               0xBE1370 /* 1.0.4r2 */,
                               0xBE6540 /* 1.0.6 */,
                               0xBE7540 /* 1.0.7 */,
                               0xC95FD8 /* 1.0.8 */,
                               //Complete Edition EXEs
                               0xC5B33C /* 1.2.0.32 */,
                               0xC5B73C /* 1.2.0.59 */,
                               //Russian EXEs
                               0xB5B65C /* 1.0.0.1 */,
                               0xB569F4 /* 1.0.1.1 */,
                               0xB76CB4 /* 1.0.2.1 */,
                               0xB7AEFC /* 1.0.3.1 */,
                               //Japan EXEs
                               0xB8813C /* 1.0.1.2 */,
                               0xB8C38C /* 1.0.2.2 */,
                               0xBE6510 /* 1.0.5.2 */,
                           };
            }
        }
    }
}
