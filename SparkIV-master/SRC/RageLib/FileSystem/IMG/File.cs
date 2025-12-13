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

namespace RageLib.FileSystem.IMG
{
    internal class File
    {
        private Stream _stream;
        private string _filename;

        public Header Header { get; private set; }
        public TOC TOC { get; private set; }

        public bool Open(string filename)
        {
            Header = new Header(this);
            TOC = new TOC(this);
            _filename = filename;

            bool encrypted = false;

            _stream = new FileStream(filename, FileMode.Open, FileAccess.ReadWrite, FileShare.ReadWrite);

            var br = new BinaryReader(_stream);

            byte[] headerData = br.ReadBytes(0x14);

            if (BitConverter.ToUInt32(headerData, 0) != Header.MagicId)
            {
                encrypted = true;
            }

            if (encrypted)
            {
                headerData = DataUtil.Decrypt(headerData);
            }

            var headerMS = new MemoryStream(headerData);

            Header.Read(new BinaryReader(headerMS));

            headerMS.Close();

            if (Header.Identifier != Header.MagicId || Header.Version != Header.SupportedVersion)
            {
                _stream.Close();
                return false;
            }

            byte[] tocData = br.ReadBytes(Header.TocSize);

            if (encrypted)
            {
                tocData = DataUtil.Decrypt(tocData);
            }

            var tocMS = new MemoryStream(tocData);

            TOC.Read(new BinaryReader(tocMS));

            tocMS.Close();

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
                var tocOffset = _stream.Position;

                // Find the max value here
                var dataOffset = int.MinValue;
                foreach (var entry in TOC)
                {
                    var offset = entry.OffsetBlock + entry.UsedBlocks;
                    if (offset > dataOffset)
                    {
                        dataOffset = offset;
                    }
                }

                foreach (var entry in TOC)
                {
                    if (entry.CustomData != null)
                    {
                        var blockCount = (int)Math.Ceiling((float)entry.CustomData.Length / TOCEntry.BlockSize);
                        if (blockCount <= entry.UsedBlocks)
                        {
                            // Clear up the old data
                            _stream.Seek(entry.OffsetBlock * TOCEntry.BlockSize, SeekOrigin.Begin);
                            bw.Write(new byte[entry.UsedBlocks * TOCEntry.BlockSize]);

                            // We can fit it in the existing block... so lets do that.
                            _stream.Seek(entry.OffsetBlock * TOCEntry.BlockSize, SeekOrigin.Begin);
                        }
                        else
                        {
                            // Clear up the old data
                            _stream.Seek(entry.OffsetBlock * TOCEntry.BlockSize, SeekOrigin.Begin);
                            bw.Write(new byte[entry.UsedBlocks * TOCEntry.BlockSize]);

                            // Fit it at the end of the stream
                            entry.OffsetBlock = dataOffset;
                            _stream.Seek(dataOffset*TOCEntry.BlockSize, SeekOrigin.Begin);
                            dataOffset += blockCount;
                        }

                        entry.UsedBlocks = (short)blockCount;

                        bw.Write(entry.CustomData);

                        if ((entry.CustomData.Length % TOCEntry.BlockSize) != 0)
                        {
                            var padding = new byte[blockCount * TOCEntry.BlockSize - entry.CustomData.Length];
                            bw.Write(padding);
                        }

                        entry.SetCustomData(null);
                    }
                }

                _stream.Seek(tocOffset, SeekOrigin.Begin);

                TOC.Write(bw);
            }
        }

        public void Rebuild()
        {
            if (Header.EntryCount > 0)
            {
                string tempFilename = _filename + ".temp";
                var tempFS = new FileStream(tempFilename, FileMode.Create, FileAccess.Write);
                try
                {
                    var bw = new BinaryWriter(tempFS);

                    Header.Write(bw);

                    // Recalculate the offset/sizes of the TOC entries
                    var tocOffset = tempFS.Position;

                    var dataOffset = TOC.GetTOCBlockSize();

                    tempFS.Seek(dataOffset * TOCEntry.BlockSize, SeekOrigin.Begin);
                    foreach (var entry in TOC)
                    {
                        if (entry.CustomData == null)
                        {
                            bw.Write(ReadData(entry.OffsetBlock * TOCEntry.BlockSize, entry.UsedBlocks * TOCEntry.BlockSize));
                        }
                        else
                        {
                            var blockCount = (int) Math.Ceiling((float) entry.CustomData.Length/TOCEntry.BlockSize);
                            entry.UsedBlocks = (short)blockCount;
                            
                            bw.Write( entry.CustomData );
                            
                            if ( (entry.CustomData.Length % TOCEntry.BlockSize) != 0 )
                            {
                                var padding = new byte[ blockCount * TOCEntry.BlockSize - entry.CustomData.Length ];
                                bw.Write(padding);
                            }
                        }
                        entry.OffsetBlock = dataOffset;
                        dataOffset += entry.UsedBlocks;
                    }

                    tempFS.Seek(tocOffset, SeekOrigin.Begin);

                    TOC.Write( bw );

                }
                finally
                {
                    tempFS.Close();
                }

                Close();
                
                System.IO.File.Delete( _filename );
                System.IO.File.Move( tempFilename, _filename );

                Open(_filename);
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