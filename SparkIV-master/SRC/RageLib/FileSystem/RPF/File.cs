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

namespace RageLib.FileSystem.RPF
{
    internal class File
    {
        private Stream _stream;

        public File()
        {
            Header = new Header(this);
            TOC = new TOC(this);
        }

        public Header Header { get; private set; }
        public TOC TOC { get; private set; }

        public bool Open(string filename)
        {
            _stream = new FileStream(filename, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite);

            var br = new BinaryReader(_stream);
            Header.Read(br);

            if (!Enum.IsDefined(typeof (MagicId), (int) Header.Identifier))
            {
                _stream.Close();
                return false;
            }

            _stream.Seek(0x800, SeekOrigin.Begin);
            TOC.Read(br);

            return true;
        }

        public void Close()
        {
            if (_stream != null)
            {
                _stream.Close();
            }
        }

        public void Save()
        {
            if (Header.EntryCount > 0)
            {
                _stream.Position = 0;

                var bw = new BinaryWriter(_stream);

                Header.Write(bw);

                // Recalculate the offset/sizes of the TOC entries
                var tocOffset = 0x800;

                // Find the max value here
                var dataOffset = int.MinValue;
                foreach (var entry in TOC)
                {
                    var fileEntry = entry as FileEntry;
                    if (fileEntry != null)
                    {
                        var offset = fileEntry.Offset + fileEntry.SizeUsed;
                        if (offset > dataOffset)
                        {
                            dataOffset = offset;
                        }                        
                    }
                }

                foreach (var entry in TOC)
                {
                    var fileEntry = entry as FileEntry;
                    if (fileEntry != null && fileEntry.CustomData != null)
                    {
                        var blockCount = (int)Math.Ceiling((float)fileEntry.CustomData.Length / FileEntry.BlockSize);
                        var blockSize = blockCount*FileEntry.BlockSize;

                        if (blockSize <= fileEntry.SizeUsed)
                        {
                            // Clear up the old data
                            _stream.Seek(fileEntry.Offset, SeekOrigin.Begin);
                            bw.Write(new byte[fileEntry.SizeUsed]);

                            // We can fit it in the existing block... so lets do that.
                            _stream.Seek(fileEntry.Offset, SeekOrigin.Begin);
                        }
                        else
                        {
                            // Clear up the old data
                            _stream.Seek(fileEntry.Offset, SeekOrigin.Begin);
                            bw.Write(new byte[fileEntry.SizeUsed]);

                            // Fit it at the end of the stream
                            fileEntry.Offset = dataOffset;
                            _stream.Seek(dataOffset, SeekOrigin.Begin);
                            dataOffset += blockSize;
                        }

                        fileEntry.SizeUsed = blockSize;

                        bw.Write(fileEntry.CustomData);

                        if ((fileEntry.CustomData.Length % FileEntry.BlockSize) != 0)
                        {
                            var padding = new byte[blockSize - fileEntry.CustomData.Length];
                            bw.Write(padding);
                        }

                        fileEntry.SetCustomData(null);
                    }
                }

                _stream.Seek(tocOffset, SeekOrigin.Begin);

                TOC.Write(bw);
            }
        }

        public byte[] ReadData(int offset, int length)
        {
            var buffer = new byte[length];
            _stream.Seek(offset, SeekOrigin.Begin);
            _stream.Read(buffer, 0, length);
            return buffer;
        }

    }
}