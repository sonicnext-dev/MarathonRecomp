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

using System.Collections.Generic;
using System.IO;

namespace RageLib.Shaders
{
    public class ShaderFile
    {
        internal ShaderFX.File File { get; private set; }
        
        public List<Shader> VertexShaders { get; private set; }
        public List<Shader> PixelShaders { get; private set; }

        public void Open(string filename)
        {
            File = new ShaderFX.File();
            File.Open(filename);

            ProcessShaders();
        }

        public void Open(Stream stream)
        {
            File = new ShaderFX.File();
            File.Open(stream);

            ProcessShaders();
        }

        private void ProcessShaders()
        {
            VertexShaders = new List<Shader>();
            foreach (var shader in File.ShaderFX.VertexShaders)
            {
                VertexShaders.Add(new Shader(shader));
            }

            PixelShaders = new List<Shader>();
            foreach (var shader in File.ShaderFX.PixelShaders)
            {
                PixelShaders.Add(new Shader(shader));
            }
        }
    }
}
