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
using RageLib.Audio.SoundBank.MultiChannel;
using RageLib.Audio.WaveFile;

namespace RageLib.Audio.SoundBank
{
    class SoundBankMultiChannel : ISoundBank, IMultichannelSound
    {
        public const int BlockSize = 2048;

        private Header _fileHeader;
        private ChannelInfoHeader[] _channelInfoHeader;
        private ChannelInfo[] _channelInfo;
        private bool _isCompressed = true;
        private BlockInfoHeader[] _blockInfoHeader;
        private BlockInfo[] _blockInfo;
        private int _sizeBlockHeader = BlockSize;

        private string _commonFileName;
        private int _commonSampleRate;
        private ChannelMask _channelMask;
        private int[] _channelOrder;
        private bool _supportsMultichannelExport;

        private List<ISoundWave> _waveInfos;

        private bool ReorganizeForMultiChannelWave()
        {
            // check if stereo, 3 channel or 5 channel...
            if (!(_fileHeader.numChannels == 2 || _fileHeader.numChannels == 3 || _fileHeader.numChannels == 5)) return false;

            // check if all channels have names to guess order....
            for (int i = 0; i < _channelInfoHeader.Length; i++)
            {
                if (_channelInfo[i].Name == null || !(_channelInfo[i].Name.Contains("_") || _channelInfo[i].Name.Contains(".")))
                {
                    return false;
                }
            }

            // extract common name and channel postfixes
            int[] tmpChannelOrder = new int[_fileHeader.numChannels];
            string[] tmpChannelNames = new string[_fileHeader.numChannels];
            for (int i = 0; i < _fileHeader.numChannels; i++)
            {
                string channelName = _channelInfo[i].Name;

                // get unique parts of name
                int pos = Math.Max(channelName.LastIndexOf('.'), _channelInfo[i].Name.LastIndexOf('_'));
                tmpChannelNames[i] = channelName.Substring(pos + 1);
                string tmpString = channelName.Substring(0, pos);

                // check if common part of name is the same for all channels
                if (_commonFileName == null)
                {
                    _commonFileName = tmpString;
                }
                else if (_commonFileName != tmpString)
                {
                    return false;
                }

                // check if all channels have the same sampleRate
                if (_commonSampleRate == 0)
                {
                    _commonSampleRate = _channelInfo[i].sampleRate;
                }
                else if (_commonSampleRate != _channelInfo[i].sampleRate)
                {
                    return false;
                }
            }

            // try to guess channel mapping, return false if inconsistent...
            _channelMask = ChannelMask.Invalid;
            for (int i = 0; i < _fileHeader.numChannels; i++)
            {
                if (tmpChannelNames[i] == "LEFT" || tmpChannelNames[i] == "L")
                {
                    _channelMask |= ChannelMask.SpeakerFrontLeft;
                    tmpChannelOrder[i] = 0;
                }
                else if (tmpChannelNames[i] == "RIGHT" || tmpChannelNames[i] == "R")
                {
                    _channelMask |= ChannelMask.SpeakerFrontRight;
                    tmpChannelOrder[i] = 1;
                }
                else if (tmpChannelNames[i] == "CENTRE" || tmpChannelNames[i] == "C")
                {
                    if (_fileHeader.numChannels < 3)
                    {
                        return false;
                    }
                    _channelMask |= ChannelMask.SpeakerFrontCenter;
                    tmpChannelOrder[i] = 2;
                }
                else if (tmpChannelNames[i] == "LS")
                {
                    if (_fileHeader.numChannels != 5)
                    {
                        return false;
                    }
                    _channelMask |= ChannelMask.SpeakerBackLeft;
                    tmpChannelOrder[i] = 3;
                }
                else if (tmpChannelNames[i] == "RS")
                {
                    if (_fileHeader.numChannels != 5)
                    {
                        return false;
                    }
                    _channelMask |= ChannelMask.SpeakerBackRight;
                    tmpChannelOrder[i] = 4;
                }
                else
                {
                    return false;
                }
            }
            
            // it's important to export samples in the right order: L, R, C, BL, BR - this array is for reordering...
            _channelOrder = tmpChannelOrder;
            return true;
        }

        #region Implementation of ISoundBank

        public int WaveCount
        {
            get { return _waveInfos.Count; }
        }

        public ISoundWave this[int index]
        {
            get { return _waveInfos[index]; }
        }

        public int ExportWaveBlockAsPCM(int waveIndex, int blockIndex, ref DviAdpcmDecoder.AdpcmState state, Stream soundBankStream, Stream outStream)
        {
            // waveIndex would be the channel here...
            // blockIndex is the real block index

            BinaryWriter writer = new BinaryWriter(outStream);

            long offset = _blockInfo[blockIndex].computed_offset + _sizeBlockHeader;
            soundBankStream.Seek(offset, SeekOrigin.Begin);

            if (_isCompressed)
            {
                for (int i = 0; i < _blockInfo[blockIndex].codeIndices.Length; i++)
                {
                    int currentChannel = _blockInfo[blockIndex].codeIndices[i].computed_channel;
                    if (currentChannel == waveIndex)
                    {
                        int adpcmIndex = _blockInfo[blockIndex].codeIndices[i].computed_adpcmIndex;
                        if (adpcmIndex < _channelInfo[currentChannel].adpcmInfo.states.Length)
                        {
                            state = _channelInfo[currentChannel].adpcmInfo.states[adpcmIndex];
                        }

                        byte[] buffer = new byte[BlockSize];
                        soundBankStream.Read(buffer, 0, BlockSize);

                        for (int j = 0; j < BlockSize; j++)
                        {
                            byte code = buffer[j];
                            writer.Write(DviAdpcmDecoder.DecodeAdpcm((byte) (code & 0xf), ref state));
                            writer.Write(DviAdpcmDecoder.DecodeAdpcm((byte) ((code >> 4) & 0xf), ref state));
                        }
                    }
                    else
                    {
                        soundBankStream.Seek(2048, SeekOrigin.Current);
                    }
                }

            }
            else
            {
                for (int i = 0; i < _blockInfo[blockIndex].codeIndices.Length; i++)
                {
                    int currentChannel = _blockInfo[blockIndex].codeIndices[i].computed_channel;

                    if (currentChannel == waveIndex)
                    {
                        short[] block = new short[BlockSize / 2];

                        // all the seeking is done here...
                        soundBankStream.Seek(_blockInfo[blockIndex].computed_offset + i * BlockSize + _sizeBlockHeader, SeekOrigin.Begin);
                        for (int j = 0; j < BlockSize / 2; j++)
                        {
                            byte[] b = new byte[2];
                            soundBankStream.Read(b, 0, 2);
                            block[j] = BitConverter.ToInt16(b, 0);
                        }

                        // this adjusts for weird values in codeIndices.
                        if (_blockInfo[blockIndex].channelInfo[currentChannel].offsetIntoCodeBlockIndices < 0)
                        {
                            _blockInfo[blockIndex].codeIndices[i].startIndex -=
                                _blockInfo[blockIndex].channelInfo[currentChannel].offsetIntoCodeBlockIndices;
                            _blockInfo[blockIndex].channelInfo[currentChannel].offsetIntoCodeBlockIndices = 0;
                        }
                        else if (_blockInfo[blockIndex].channelInfo[currentChannel].offsetIntoCodeBlockIndices > 0)
                        {
                            int len = _blockInfo[blockIndex].channelInfo[currentChannel].offsetIntoCodeBlockIndices;
                            short[] newblock = new short[BlockSize / 2 - len];
                            Array.Copy(block, len, newblock, 0, BlockSize / 2 - len);
                            block = newblock;
                            _blockInfo[blockIndex].channelInfo[currentChannel].offsetIntoCodeBlockIndices = 0;
                        }

                        int count = _blockInfo[blockIndex].codeIndices[i].endIndex -
                                    _blockInfo[blockIndex].codeIndices[i].startIndex;
                        for (int j = 0; j <= count; j++)
                        {
                            writer.Write(block[j]);
                        }
                    }
                }
            }
            return 0;

        }

        public void ExportAsPCM(int index, Stream soundBankStream, Stream outStream)
        {
            int count = _fileHeader.numBlocks;

            DviAdpcmDecoder.AdpcmState state = new DviAdpcmDecoder.AdpcmState();

            for (int k = 0; k < count; k++)
            {
                ExportWaveBlockAsPCM(index, k, ref state, soundBankStream, outStream);
            }
        }

        #endregion

        #region Implementation of IMultichannelSound

        public void ExportMultichannelAsPCM(Stream soundBankStream, Stream outStream)
        {
            int numBlocks = _fileHeader.numBlocks;
            int numChannels = _fileHeader.numChannels;
            DviAdpcmDecoder.AdpcmState[] state = new DviAdpcmDecoder.AdpcmState[numChannels];

            BinaryWriter bw = new BinaryWriter(outStream);

            int[] inverseChannelOrder = new int[numChannels];
            for (int i = 0; i < numChannels; i++)
            {
                inverseChannelOrder[_channelOrder[i]] = i;
            }

            for (int i = 0; i < numChannels; i++)
            {
                state[i] = new DviAdpcmDecoder.AdpcmState();
            }

            for (int blockIndex = 0; blockIndex < numBlocks; blockIndex++)
            {
                byte[][] blockData = new byte[numChannels][];
                
                // Decode the block for all channels
                for (int channelIndex = 0; channelIndex < numChannels; channelIndex++)
                {
                    MemoryStream ms = new MemoryStream();
                    ExportWaveBlockAsPCM(channelIndex, blockIndex, ref state[channelIndex], soundBankStream, ms);
                    blockData[channelIndex] = ms.ToArray();
                }

                // Now interleave them
                for (int j = 0; j < blockData[0].Length / 2; j++)
                {
                    for (int i = 0; i < numChannels; i++)
                    {
                        bw.Write(blockData[inverseChannelOrder[i]][j*2 + 0]);
                        bw.Write(blockData[inverseChannelOrder[i]][j*2 + 1]);
                    }
                }
            }
        }

        public int CommonSamplesPerSecond
        {
            get { return _commonSampleRate; }
        }

        public string CommonFilename
        {
            get { return _commonFileName; }
        }

        public ChannelMask ChannelMask
        {
            get { return _channelMask; }
        }

        public bool SupportsMultichannelExport
        {
            get { return _supportsMultichannelExport; }
        }

        #endregion

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            // Read header

            _fileHeader = new Header(br);

            bool errorCondition = false;

            if (!(_fileHeader.numBlocks > 0)) errorCondition = true;
            if (!(_fileHeader.sizeBlock > 0)) errorCondition = true;
            if (_fileHeader.numBlocks * _fileHeader.sizeBlock > br.BaseStream.Length) errorCondition = true;
            if (_fileHeader.offsetBlockInfo > _fileHeader.sizeHeader) errorCondition = true;

            if (errorCondition)
            {
                throw new SoundBankException("Unexpected values in Header");
            }

            // read adpcm state info

            br.BaseStream.Seek(_fileHeader.offsetChannelInfo, SeekOrigin.Begin);
            _channelInfoHeader = new ChannelInfoHeader[_fileHeader.numChannels];
            for (int i = 0; i < _fileHeader.numChannels; i++)
            {
                _channelInfoHeader[i] = new ChannelInfoHeader(br);
            }
            
            _channelInfo = new ChannelInfo[_fileHeader.numChannels];
            var currentOffset = br.BaseStream.Position;

            for (int i = 0; i < _fileHeader.numChannels; i++)
            {
                br.BaseStream.Seek(currentOffset + _channelInfoHeader[i].offset, SeekOrigin.Begin);
                _channelInfo[i] = new ChannelInfo(br);

                if (_channelInfoHeader[i].size <= 36)
                {
                    _isCompressed = false;
                }
                else
                {
                    _channelInfo[i].adpcmInfo = new AdpcmInfo(br);
                }
            }

            // Read comp block info (in header)
            br.BaseStream.Seek(_fileHeader.offsetBlockInfo, SeekOrigin.Begin);
            _blockInfoHeader = new BlockInfoHeader[_fileHeader.numBlocks];
            for (int i = 0; i < _fileHeader.numBlocks; i++)
            {
                _blockInfoHeader[i] = new BlockInfoHeader(br);
            }

            // Read comp block info / channel info
            _blockInfo = new BlockInfo[_fileHeader.numBlocks];
            for (int i = 0; i < _fileHeader.numBlocks; i++)
            {
                var computedOffset = _fileHeader.sizeHeader + _fileHeader.sizeBlock*i;
                br.BaseStream.Seek(computedOffset, SeekOrigin.Begin);

                _blockInfo[i] = new BlockInfo(br);
                _blockInfo[i].computed_offset = computedOffset;

                _blockInfo[i].channelInfo = new BlockChannelInfo[_fileHeader.numChannels];
                int numCodeIndices = 0;
                for(int j=0; j<_fileHeader.numChannels; j++)
                {
                    _blockInfo[i].channelInfo[j] = new BlockChannelInfo(br);
                    
                    int end = _blockInfo[i].channelInfo[j].startIndex + _blockInfo[i].channelInfo[j].count;
                    if (numCodeIndices < end)
                    {
                        numCodeIndices = end;
                    }
                }

                _blockInfo[i].codeIndices = new CodeIndices[numCodeIndices];
                for (int j = 0; j < numCodeIndices; j++)
                {
                    _blockInfo[i].codeIndices[j] = new CodeIndices(br);
                    _blockInfo[i].codeIndices[j].computed_channel = -1;

                    if (_isCompressed)
                    {
                        _blockInfo[i].codeIndices[j].computed_adpcmIndex = _blockInfo[i].codeIndices[j].startIndex/
                                                                            4096;
                    }
                }

                for (int j = 0; j < _fileHeader.numChannels; j++)
                {
                    int channelIdxStart = _blockInfo[i].channelInfo[j].startIndex;
                    int channelIdxCount = _blockInfo[i].channelInfo[j].count;
                    for (int k = 0; k < channelIdxCount; k++)
                    {
                        _blockInfo[i].codeIndices[k + channelIdxStart].computed_channel = j;
                    }
                }

                int new_start =
                    (int) (Math.Ceiling((float) (_blockInfo[i].offset2 + 8*numCodeIndices)/BlockSize)*BlockSize);
                _sizeBlockHeader = Math.Max(_sizeBlockHeader, new_start);
            }

            _waveInfos = new List<ISoundWave>(_fileHeader.numChannels);
            for (int i = 0; i < _fileHeader.numChannels; i++)
            {
                _waveInfos.Add( new SoundWave(_fileHeader, _channelInfo[i]) );
            }

            _supportsMultichannelExport = ReorganizeForMultiChannelWave();
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion

    }
}
