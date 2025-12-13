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

using System.Reflection;

namespace RageLib.Common.Resources
{
    [Obfuscation(StripAfterObfuscation = true, ApplyToMembers = true, Exclude = true)]
    public enum ResourceType
    {
        TextureXBOX = 0x7, // xtd
        ModelXBOX = 0x6D, // xdr
        Generic = 0x01, // xhm / xad (Generic files as rsc?)
        Bounds = 0x20, // xbd, wbd
        Particles = 0x24, // xpfl
        Particles2 = 0x1B, // xpfl

        Texture = 0x8, // wtd
        Model = 0x6E, // wdr
        ModelFrag = 0x70, //wft
    }
}