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

using System.IO;
using RageLib.Audio.SoundBank.Hashes;
using RageLib.Common;

namespace RageLib.Audio.SoundBank.Mono
{
    internal struct WaveInfo : IFileAccess, ISoundWave
    {
        public long offset;
        public uint hash;
        public int numSamplesInBytes;
        public int numSamples16Bit;
        public int unk5;
        public ushort samplerate;
        public ushort unk6;
        public int unk7;
        public long offsetToStates;
        public uint numSamples16Bit2;
        public uint unk11;
        public uint unk12;
        public int numStates;

        public DviAdpcmDecoder.AdpcmState[] states;

        public int numSamplesInBytes_computed;
        public bool is_compressed;

        public WaveInfoHeader Header { get; set; }

        public string Name
        {
            get
            {
                return HashResolver.Resolve(hash);
            }
        }

        public int NumberOfSamples
        {
            get { return numSamples16Bit; }
        }

        public int SamplesPerSecond
        {
            get { return samplerate; }
        }

        public int BlockSize
        {
            get { return 2048; }
        }

        public int BlockCount
        {
            get { return numSamplesInBytes_computed/BlockSize; }
        }

        public WaveInfo(WaveInfoHeader header) : this()
        {
            Header = header;
        }

        public WaveInfo(WaveInfoHeader header, BinaryReader br) : this()
        {
            Header = header;
            Read(br);
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            offset = br.ReadInt64();
            hash = br.ReadUInt32();
            numSamplesInBytes = br.ReadInt32();
            numSamplesInBytes_computed = SoundBankMono.GetPaddedSize(numSamplesInBytes);
            numSamples16Bit = br.ReadInt32();
            unk5 = br.ReadInt32();
            samplerate = br.ReadUInt16();
            unk6 = br.ReadUInt16();
                
            if (Header.size > 32)
            {
                unk7 = br.ReadInt32();
                offsetToStates = br.ReadInt64();
                numSamples16Bit2 = br.ReadUInt32();
                unk11 = br.ReadUInt32();
                unk12 = br.ReadUInt32();
                numStates = br.ReadInt32();
                if (numStates > 0)
                {
                    is_compressed = true;
                    states = new DviAdpcmDecoder.AdpcmState[numStates];
                    for (int j = 0; j < numStates; j++)
                    {
                        DviAdpcmDecoder.AdpcmState state = new DviAdpcmDecoder.AdpcmState();
                        state.valprev = br.ReadInt16();
                        state.index = br.ReadByte();
                        states[j] = state;
                    }
                }
            }

        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}