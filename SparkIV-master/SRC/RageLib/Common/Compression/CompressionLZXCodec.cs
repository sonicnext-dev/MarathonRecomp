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

namespace RageLib.Common.Compression
{
    internal class CompressionLZXCodec : ICompressionCodec
    {
        public void Compress(Stream source, Stream destination)
        {
            throw new NotImplementedException();
        }

        public void Decompress(Stream source, Stream destination)
        {
            var br = new BinaryReader(source);

            if (br.ReadUInt16() != 0xEF12)
            {
                throw new Exception("Unexpected input in compressed resource file.");
            }

            throw new NotImplementedException();

            /*
            
            // The following code requires code that is is ommitted due to licensing issues.
              
            var length = DataUtil.SwapEndian(br.ReadUInt32());

            var decompressor = new LZXDecompressor();

            decompressor.Initialize(17);

            uint bytesRead = 0;
            while (bytesRead < length)
            {
                uint inChunkSize, outChunkSize;

                inChunkSize = br.ReadByte();
                if (inChunkSize != 0xFF)
                {
                    inChunkSize <<= 8;
                    inChunkSize |= br.ReadByte();
                    outChunkSize = 0x8000;

                    bytesRead += 2;
                }
                else
                {
                    outChunkSize = ((uint) br.ReadByte() << 8) | (br.ReadByte());
                    inChunkSize = ((uint) br.ReadByte() << 8) | (br.ReadByte());

                    bytesRead += 5;
                }

                byte[] inData = br.ReadBytes((int) inChunkSize);
                var outData = new byte[outChunkSize];

                bytesRead += inChunkSize;

                try
                {
                    decompressor.Decompress(inData, outData, (int) inChunkSize, (int) outChunkSize);
                }
                catch
                {
                    throw new Exception("Could not decompress resource.");
                }

                destination.Write(outData, 0, (int) outChunkSize);
            }
             */
        }
    }
}