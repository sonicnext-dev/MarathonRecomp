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
using RageLib.Common.Compression;

namespace RageLib.Common.Resources
{
    public class ResourceFile : IDisposable
    {
        private ResourceHeader _header;
        private ICompressionCodec _codec;
        private byte[] _systemMemData;
        private byte[] _graphicsMemData;

        public CompressionType Compression
        {
            get { return _header.CompressCodec; }
        }

        public ResourceType Type
        {
            get { return _header.Type; }
        }

        public int SystemMemSize
        {
            get { return _header.GetSystemMemSize(); }
        }

        public int GraphicsMemSize
        {
            get { return _header.GetGraphicsMemSize(); }
        }

        public byte[] SystemMemData
        {
            get { return _systemMemData; }
            set
            {
                var data = value;
                _header.SetMemSizes( data.Length, GraphicsMemSize );
                
                _systemMemData = new byte[SystemMemSize];
                data.CopyTo(_systemMemData, 0);
            }
        }

        public byte[] GraphicsMemData
        {
            get { return _graphicsMemData; }
            set
            {
                var data = value;
                _header.SetMemSizes(SystemMemSize, data.Length);

                _graphicsMemData = new byte[GraphicsMemSize];
                data.CopyTo(_graphicsMemData, 0);
            }
        }

        public void Read(Stream data)
        {
            _header = new ResourceHeader();

            var br = new BinaryReader(data);
            _header.Read(br);
            if (_header.Magic != ResourceHeader.MagicValue)
            {
                throw new Exception("Not a valid resource");
            }

            switch (_header.CompressCodec)
            {
                case CompressionType.LZX:
                    _codec = CompressionCodecFactory.LZX;
                    break;
                case CompressionType.Deflate:
                    _codec = CompressionCodecFactory.Deflate;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            var ms = new MemoryStream();
            _codec.Decompress( data, ms );

            ms.Seek(0, SeekOrigin.Begin);

            _systemMemData = new byte[SystemMemSize];
            ms.Read(_systemMemData, 0, SystemMemSize);

            _graphicsMemData = new byte[GraphicsMemSize];
            ms.Read(_graphicsMemData, 0, GraphicsMemSize);

            ms.Close();
        }

        public void Write(Stream data)
        {
            var bw = new BinaryWriter(data);
            
            if (SystemMemSize != _systemMemData.Length || GraphicsMemSize != _graphicsMemData.Length)
            {
                _header.SetMemSizes(SystemMemSize, GraphicsMemSize);
            }

            _header.Write( bw );

            var ms = new MemoryStream();
            ms.Write(_systemMemData, 0, _systemMemData.Length);
            ms.Write(new byte[SystemMemSize - _systemMemData.Length], 0, SystemMemSize - _systemMemData.Length);
            ms.Write(_graphicsMemData, 0, _graphicsMemData.Length);
            ms.Write(new byte[GraphicsMemSize - _graphicsMemData.Length], 0, GraphicsMemSize - _graphicsMemData.Length);

            ms.Seek(0, SeekOrigin.Begin);

            var msCompress = new MemoryStream();

            _codec.Compress( ms, msCompress );

            bw.Write(msCompress.ToArray());

            ms.Close();
            msCompress.Close();

            bw.Flush();
        }

        #region Implementation of IDisposable

        public void Dispose()
        {
            _systemMemData = null;
            _graphicsMemData = null;
        }

        #endregion
    }
}