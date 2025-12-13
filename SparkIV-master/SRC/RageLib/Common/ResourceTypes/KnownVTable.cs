/**********************************************************************\

 RageLib
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

namespace RageLib.Common.ResourceTypes
{
    public enum KnownVTable
    {
        VTable_pgDictionary_gtaDrawable = 0x6953A4,
        VTable_pgDictionary_grcTexturePC = 0x6A08A0,
        VTable_gtaFragType = 0x695238,
        VTable_gtaDrawable = 0x695254,
        VTable_fragDrawable = 0x6A32DC,
        VTable_grmShaderGroup = 0x6B1644,
        VTable_grmShaderFx = 0x6B223C,
        VTable_grmModel = 0x6B0234,
        VTable_grmGeometry = 0x6B48F4,
        VTable_grcVertexBufferD3D = 0x6BBAD8,
        VTable_grcIndexBufferD3D = 0x6BB870,
        VTable_grcTexture = 0x6B675C,
        VTable_grcTexturePC = 0x6B1D94,
        VTable_phArchetypeDamp = 0x69A5BC,
        VTable_phBoundComposite = 0x69BBEC,
        VTable_phBoundBox = 0x69D56C,
        VTable_phBoundGeometry = 0x69AAF4,
        VTable_phBoundCurvedGeometry = 0x69B41C,
        VTable_evtSet = 0x6A4678,
    }
}