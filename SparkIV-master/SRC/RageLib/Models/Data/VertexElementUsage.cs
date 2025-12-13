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
    public enum VertexElementUsage
    {
        Position = 0,
        BlendWeight = 1,
        BlendIndices = 2,
        Normal = 3,
        PointSize = 4,
        TextureCoordinate = 5,
        Tangent = 6,
        Binormal = 7,
        TesselateFactor = 8,
        PositionT = 9,
        Color = 10,
        Fog = 11,
        Depth = 12,
        Sample = 13,
    }
}