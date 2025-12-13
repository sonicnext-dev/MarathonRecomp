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
using System.IO;
using RageLib.Common;

namespace RageLib.Shaders.ShaderFX
{
    internal class VertexShader : IFileAccess
    {
        public Variables Variables { get; private set; }
        public int Size { get; private set; }
        private int CompressedSize { get; set; }
        public byte[] ShaderData { get; private set; }

        public VertexShader()
        {
        }

        public VertexShader(BinaryReader br)
        {
            Read(br);
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            Variables = new Variables(br);
            Size = br.ReadInt16();
            CompressedSize = br.ReadInt16();

            if (Size != CompressedSize)
            {
                throw new Exception("Encountered a shader file with a compressed VertexShader");
            }

            ShaderData = br.ReadBytes(Size);
        }

        public void Write(BinaryWriter bw)
        {
            
        }

        #endregion
    }
}