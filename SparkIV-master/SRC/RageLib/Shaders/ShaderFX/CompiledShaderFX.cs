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

using System;
using System.Collections.Generic;
using System.IO;
using RageLib.Common;

namespace RageLib.Shaders.ShaderFX
{
    internal class CompiledShaderFX : IFileAccess
    {
        private const int ValidMagic = 0x61786772;

        public List<VertexShader> VertexShaders { get; private set; }
        public List<PixelShader> PixelShaders { get; private set; }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            if (br.ReadInt32() != ValidMagic)
            {
                throw new Exception("Not a valid shader file");
            }

            int vsCount = br.ReadByte();
            VertexShaders = new List<VertexShader>(vsCount);
            for(int i=0; i<vsCount; i++)
            {
                VertexShaders.Add( new VertexShader(br) );
            }
            
            int psCount = br.ReadByte();
            PixelShaders = new List<PixelShader>(psCount);
            for (int i = 0; i < psCount; i++)
            {
                PixelShaders.Add(new PixelShader(br));
            }

            // More stuff here that isn't be read right now... but however are documented.
            // 3 more logical sections to come.
        }

        public void Write(BinaryWriter bw)
        {
            
        }

        #endregion
    }
}