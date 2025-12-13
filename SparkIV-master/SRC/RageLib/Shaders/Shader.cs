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
using RageLib.Shaders.ShaderFX;

namespace RageLib.Shaders
{
    public class Shader
    {
        public ShaderType Type { get; private set; }
        public List<string> Variables { get; private set; }
        public byte[] Data { get; private set; }

        internal Shader(VertexShader shader)
        {
            Type = ShaderType.Vertex;
            Variables = new List<string>( shader.Variables.Definitions.Count );
            foreach (var definition in shader.Variables.Definitions)
            {
                Variables.Add(definition.VariableName);
            }
            Data = shader.ShaderData;
        }

        internal Shader(PixelShader shader)
        {
            Type = ShaderType.Pixel;
            Variables = new List<string>(shader.Variables.Definitions.Count);
            foreach (var definition in shader.Variables.Definitions)
            {
                Variables.Add(definition.VariableName);
            }
            Data = shader.ShaderData;
        }
    }
}
