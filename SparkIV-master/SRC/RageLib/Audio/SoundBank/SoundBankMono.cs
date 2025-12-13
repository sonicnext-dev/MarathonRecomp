/**********************************************************************\

 RageLib - Audio
 
 Copyright (C) 2009  DerPlaya78
 Portions Copyright (C) 2009  Arushan/Aru <oneforaru at gmail.com>

 Modified and adapted for RageLib from iv_audio_rip
 
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
using RageLib.Audio.SoundBank.Mono;

namespace RageLib.Audio.SoundBank
{
    class SoundBankMono : ISoundBank
    {
        internal static int GetPaddedSize(int input)
        {
            return (int)(Math.Ceiling(input / 2048f) * 2048f);
        }

        public Header Header { get; set; }

        private List<WaveInfo> _waveInfos;

        public int WaveCount
        {
            get { return Header.numBlocks; }
        }

        public ISoundWave this[int index]
        {
            get { return _waveInfos[index];  }
        }

        public int ExportWaveBlockAsPCM(int waveIndex, int blockIndex, ref DviAdpcmDecoder.AdpcmState state, Stream soundBankStream, Stream outStream)
        {
            int samplesWritten = 0;

            WaveInfo waveInfo = _waveInfos[waveIndex];
            BinaryWriter bw = new BinaryWriter(outStream);
            byte[] block = new byte[2048];

            int blockSize = 2048;
            if (blockIndex == (waveInfo.numSamplesInBytes_computed / blockSize) - 1)
            {
                // Last block
                blockSize = waveInfo.numSamplesInBytes%blockSize;
            }

            if (waveInfo.states != null && blockIndex < waveInfo.states.Length)
            {
                state = waveInfo.states[blockIndex];
            }

            soundBankStream.Seek(Header.headerSize + waveInfo.offset + blockIndex * 2048, SeekOrigin.Begin);
            soundBankStream.Read(block, 0, blockSize);

            int nibblePairCount = 0;

            while (nibblePairCount < blockSize)
            {
                if (waveInfo.is_compressed)
                {
                    bw.Write(DviAdpcmDecoder.DecodeAdpcm((byte)(block[nibblePairCount] & 0xf), ref state));
                    bw.Write(DviAdpcmDecoder.DecodeAdpcm((byte)((block[nibblePairCount] >> 4) & 0xf), ref state));
                    samplesWritten += 2;
                    nibblePairCount++;
                }
                else
                {
                    bw.Write(BitConverter.ToInt16(block, nibblePairCount));
                    samplesWritten++;
                    nibblePairCount += 2;
                }
            }

            return samplesWritten * 2;  // byte size
        }

        public void ExportAsPCM(int index, Stream soundBankStream, Stream outStream)
        {
            WaveInfo waveInfo = _waveInfos[index];

            int count = waveInfo.numSamplesInBytes_computed / 2048;
            
            DviAdpcmDecoder.AdpcmState state = new DviAdpcmDecoder.AdpcmState();

            for (int k = 0; k < count; k++)
            {
                ExportWaveBlockAsPCM(index, k, ref state, soundBankStream, outStream);
            }
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            // Read and validate header

            Header = new Mono.Header(br);

            if (Header.offsetWaveInfo > Header.headerSize)
            {
                throw new SoundBankException("WaveInfo is outside of header");
            }

            // Read block info headers

            var blockInfoHeaders = new WaveInfoHeader[Header.numBlocks];
            br.BaseStream.Seek(Header.offsetWaveInfo, SeekOrigin.Begin);

            for (int i = 0; i < Header.numBlocks; i++)
            {
                blockInfoHeaders[i] = new WaveInfoHeader(br);
            }

            // Read block infos
            _waveInfos = new List<WaveInfo>(Header.numBlocks);
            var position = br.BaseStream.Position;

            foreach (var biHeader in blockInfoHeaders)
            {
                br.BaseStream.Seek(position + biHeader.offset, SeekOrigin.Begin);

                var blockInfo = new WaveInfo(biHeader, br);
                _waveInfos.Add(blockInfo);
            }
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}
