/**********************************************************************\

 RageLib - Shaders
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

using System.IO;

namespace RageLib.Shaders.ShaderFX
{
    class File
    {
        internal CompiledShaderFX ShaderFX;

        public void Open(string filename)
        {
            using (var fs = new FileStream(filename, FileMode.Open, FileAccess.Read))
            {
                Open(fs);
            }
        }

        public void Open(Stream stream)
        {
            var br = new BinaryReader(stream);
            ShaderFX = new CompiledShaderFX();
            ShaderFX.Read(br);
        }
    }
}
