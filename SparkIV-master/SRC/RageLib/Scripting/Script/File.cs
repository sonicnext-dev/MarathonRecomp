/**********************************************************************\

 RageLib
 Copyright (C) 2008  Arushan/Aru <oneforaru at gmail.com>

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

namespace RageLib.Scripting.Script
{
    internal class File
    {
        public File()
        {
            Header = new Header(this);
        }

        public Header Header { get; set; }
        public byte[] Code { get; set; }
        public uint[] LocalVars { get; set; }
        public uint[] GlobalVars { get; set; }

        public bool Open(string filename)
        {
            var fs = new FileStream(filename, FileMode.Open, FileAccess.Read);
            return Open(fs);
        }

        public bool Open(Stream stream)
        {
            var br = new BinaryReader(stream);

            Header.Read(br);

            if (Header.Identifier != Header.Magic && 
                Header.Identifier != Header.MagicEncrypted && 
                Header.Identifier != Header.MagicEncryptedCompressed)
            {
                stream.Close();
                return false;
            }

            byte[] code, data1, data2;
            bool encrypted = Header.Identifier == Header.MagicEncrypted;
            bool encryptedCompressed = Header.Identifier == Header.MagicEncryptedCompressed;

            if (encryptedCompressed)
            {
                byte[] encryptedData = br.ReadBytes(Header.CompressedSize);
                byte[] compressedData = DataUtil.Decrypt(encryptedData);

                IntPtr handle = RageZip.InflateInit(compressedData, compressedData.Length);
                
                code = new byte[Header.CodeSize];
                RageZip.InflateProcess(handle, code, code.Length);

                data1 = new byte[Header.LocalVarCount*4];
                RageZip.InflateProcess(handle, data1, data1.Length);

                data2 = new byte[Header.GlobalVarCount * 4];
                RageZip.InflateProcess(handle, data2, data2.Length);

                RageZip.InflateEnd(handle);
            }
            else
            {
                code = br.ReadBytes(Header.CodeSize);
                data1 = br.ReadBytes(Header.LocalVarCount * 4);
                data2 = br.ReadBytes(Header.GlobalVarCount * 4);

                if (encrypted)
                {
                    code = DataUtil.Decrypt(code);
                    data1 = DataUtil.Decrypt(data1);
                    data2 = DataUtil.Decrypt(data2);
                }
            }

            Code = code;

            LocalVars = new uint[Header.LocalVarCount];
            for (int i = 0; i < Header.LocalVarCount; i++)
            {
                LocalVars[i] = BitConverter.ToUInt32(data1, i*4);
            }

            GlobalVars = new uint[Header.GlobalVarCount];
            for (int i = 0; i < Header.GlobalVarCount; i++)
            {
                GlobalVars[i] = BitConverter.ToUInt32(data2, i*4);
            }

            stream.Close();

            return true;
        }
    }
}