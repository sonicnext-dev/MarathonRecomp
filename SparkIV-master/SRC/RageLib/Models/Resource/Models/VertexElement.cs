/**********************************************************************\

 RageLib - Models
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

namespace RageLib.Models.Resource.Models
{
    internal struct VertexElement
    {
        public int StreamIndex;
        public VertexElementUsage Usage;
        public int UsageIndex;
        public int Size;
        public VertexElementType Type;
        /*
        // The following fields are used for some other purposes, and we don't really need them here
        public int F14;
        public short F18;
        public short F1A;
         */
    }
}