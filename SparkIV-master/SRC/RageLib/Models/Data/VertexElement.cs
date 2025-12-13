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

namespace RageLib.Models.Data
{
    public struct VertexElement
    {
        public static readonly VertexElement End = new VertexElement()
                                                       {
                                                           Stream = -1,
                                                           Offset = 0,
                                                           Type = VertexElementType.Unused,
                                                           Method = 0,
                                                           Usage = 0,
                                                           UsageIndex = 0
                                                       };

        public int Stream { get; set; }
        public int Size { get; set; }  // Not part of any D3D struct, but we include it here
        public int Offset { get; set; }
        public VertexElementType Type { get; set; }
        public VertexElementMethod Method { get; set; }
        public VertexElementUsage Usage { get; set; }
        public int UsageIndex { get; set; }
    }
}