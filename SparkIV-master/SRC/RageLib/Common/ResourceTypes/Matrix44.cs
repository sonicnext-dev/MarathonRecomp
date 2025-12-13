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

using System.IO;

namespace RageLib.Common.ResourceTypes
{
    public struct Matrix44 : IFileAccess
    {
        private float[] M;

        public static Matrix44 Identity
        {
            get
            {
                var m = new Matrix44
                                 {
                                     M = new[]
                                             {
                                                 1f, 0f, 0f, 0f,
                                                 0f, 1f, 0f, 0f,
                                                 0f, 0f, 1f, 0f,
                                                 0f, 0f, 0f, 1f,
                                             }
                                 };
                return m;
            }
        }

        public float this[int i, int j]
        {
            get { return M[i*4 + j]; }
            set { M[i*4 + j] = value; }
        }

        public float this[int m]
        {
            get { return M[m]; }
            set { M[m] = value; }
        }

        public Matrix44(BinaryReader br)
            : this()
        {
            Read(br);
        }

        public void Read(BinaryReader br)
        {
            M = new float[16];
            for (int i = 0; i < 16; i++)
            {
                M[i] = br.ReadSingle();
            }
        }

        public void Write(BinaryWriter bw)
        {
            for (int i = 0; i < 16; i++)
            {
                bw.Write(M[i]);
            }
        }

    }
}
