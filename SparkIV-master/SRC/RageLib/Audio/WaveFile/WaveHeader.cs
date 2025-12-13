/**********************************************************************\

 RageLib - Audio
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
using RageLib.Common;

namespace RageLib.Audio.WaveFile
{
    internal class WaveHeader : IFileAccess
    {
        // RIFF chunk
        private int RiffChunkID;
        private int RiffChunkSize;
        private int Format;

        // fmt sub-chunk
        private int FmtChunkID;
        private int FmtChunkSize;
        private ushort AudioFormat;
        private short NumChannels;
        private int SampleRate;
        private int ByteRate;
        private short BlockAlign;
        private short BitsPerSample;

        // Extra Data
        private short ExtraDataSize;
        private short ValidBitsPerSample;
        private ChannelMask AvailableChannelMask;
        private uint[] FormatGuid = {0x00000001, 0x00100000, 0xaa000080, 0x719b3800}; // KSDATAFORMAT_SUBTYPE_PCM

        // data sub-chunk
        private int DataChunkID;
        private int DataChunkSize;

        private const ushort WaveFormatExtensible = 0xFFFE;


        public WaveHeader() : this(false)
        {
        }

        public WaveHeader(bool extensible)
        {
            RiffChunkID = 0x46464952; // "RIFF"
            Format = 0x45564157; // "WAVE"

            FmtChunkID = 0x20746D66; // "fmt "
            FmtChunkSize = 0x10;
            AudioFormat = 1;
            NumChannels = 1; // will be updated later for extensible

            BlockAlign = 2; // will be updated later for extensible
            BitsPerSample = 16; // 16bit audio only for now

            DataChunkID = 0x61746164; // "data"

            if (extensible)
            {
                FmtChunkSize = 40;
                AudioFormat = WaveFormatExtensible;

                ExtraDataSize = 22;
                ValidBitsPerSample = 16; // all bits
                AvailableChannelMask = ChannelMask.Invalid;
            }
        }

        public int FileSize
        {
            set
            {
                RiffChunkSize = value - 8;
                DataChunkSize = value - HeaderSize;
            }
        }

        public int HeaderSize
        {
            get
            {
                if (ExtraDataSize > 0)
                {
                    return 68;
                }
                else
                {
                    return 44;
                }
            }
        }

        public int SamplesPerSecond
        {
            set
            {
                SampleRate = value;
                ByteRate = value*BlockAlign;
            }
        }

        public ChannelMask ChannelMask
        {
            set
            {
                AvailableChannelMask = value;

                int channels = 0;
                int mask = (int) value;
                for (var i = 0; i < 32; i++)
                {
                    if ((mask & 1) != 0)
                    {
                        channels++;
                    }
                    mask >>= 1;
                }

                NumChannels = (short) channels;

                BlockAlign = (short) ((BitsPerSample*NumChannels)/8);
                ByteRate = SampleRate*BlockAlign;
            }
        }

        #region Implementation of IFileAccess

        public virtual void Read(BinaryReader br)
        {
            RiffChunkID = br.ReadInt32();
            RiffChunkSize = br.ReadInt32();
            Format = br.ReadInt32();

            FmtChunkID = br.ReadInt32();
            FmtChunkSize = br.ReadInt32();
            AudioFormat = br.ReadUInt16();
            NumChannels = br.ReadInt16();
            SampleRate = br.ReadInt32();
            ByteRate = br.ReadInt32();
            BlockAlign = br.ReadInt16();
            BitsPerSample = br.ReadInt16();

            if (AudioFormat == WaveFormatExtensible)
            {
                ExtraDataSize = br.ReadInt16();
                ValidBitsPerSample = br.ReadInt16();
                AvailableChannelMask = (ChannelMask) br.ReadInt32();
                FormatGuid[0] = br.ReadUInt32();
                FormatGuid[1] = br.ReadUInt32();
                FormatGuid[2] = br.ReadUInt32();
                FormatGuid[3] = br.ReadUInt32();
            }

            DataChunkID = br.ReadInt32();
            DataChunkSize = br.ReadInt32();
        }

        public virtual void Write(BinaryWriter bw)
        {
            bw.Write(RiffChunkID);
            bw.Write(RiffChunkSize);
            bw.Write(Format);

            bw.Write(FmtChunkID);
            bw.Write(FmtChunkSize);
            bw.Write(AudioFormat);
            bw.Write(NumChannels);
            bw.Write(SampleRate);
            bw.Write(ByteRate);
            bw.Write(BlockAlign);
            bw.Write(BitsPerSample);

            if (ExtraDataSize > 0)
            {
                bw.Write(ExtraDataSize);
                bw.Write(ValidBitsPerSample);
                bw.Write((int) AvailableChannelMask);
                bw.Write(FormatGuid[0]);
                bw.Write(FormatGuid[1]);
                bw.Write(FormatGuid[2]);
                bw.Write(FormatGuid[3]);
            }

            bw.Write(DataChunkID);
            bw.Write(DataChunkSize);
        }

        #endregion
    }
}