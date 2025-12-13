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

using System.IO;
using ICSharpCode.SharpZipLib.Core;
using ICSharpCode.SharpZipLib.Zip.Compression;
using ICSharpCode.SharpZipLib.Zip.Compression.Streams;

namespace RageLib.Common.Compression
{
    internal class CompressionDeflateCodec : ICompressionCodec
    {
        private const int CopyBufferSize = 32*1024;    // 32kb

        public void Compress(Stream source, Stream destination)
        {
            /*
            var deflater = new DeflaterOutputStream(destination, new Deflater(Deflater.DEFAULT_COMPRESSION, true));

            var dataBuffer = new byte[CopyBufferSize];
            StreamUtils.Copy(source, deflater, dataBuffer);
             */

            var def = new Deflater(Deflater.DEFAULT_COMPRESSION, true);
            
            var inputData = new byte[source.Length - source.Position];
            source.Read(inputData, 0, inputData.Length);

            var buffer = new byte[CopyBufferSize];

            def.SetInput( inputData, 0, inputData.Length );
            def.Finish();

            while(!def.IsFinished)
            {
                int outputLen = def.Deflate(buffer, 0, buffer.Length);
                destination.Write( buffer, 0, outputLen );
            }

            def.Reset();
        }

        public void Decompress(Stream source, Stream destination)
        {
            var inflater = new InflaterInputStream(source, new Inflater(true));

            var dataBuffer = new byte[CopyBufferSize];
            StreamUtils.Copy(inflater, destination, dataBuffer);
        }
    }
}