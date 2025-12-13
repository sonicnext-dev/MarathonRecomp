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

namespace RageLib.Audio.SoundBank.MultiChannel
{
    internal struct ChannelInfo : IFileAccess
    {
        // data
        public int unk1Reserved;
        public int unk2Reserved;
        public uint hash;
        public int numSamplesInBytes;
        public int numSamples16Bit;
        public int unk4;
        public ushort sampleRate;
        public short unk5;
        public short unk6;
        public short unk7Reserved;
        public long offsetAdpcmStateTable;

        public AdpcmInfo adpcmInfo;

        public ChannelInfo(BinaryReader br) : this()
        {
            Read(br);
        }

        public string Name
        {
            get { return HashResolver.Resolve(hash); }
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            unk1Reserved = br.ReadInt32();
            unk2Reserved = br.ReadInt32();
            hash = br.ReadUInt32();
            numSamplesInBytes = br.ReadInt32();
            numSamples16Bit = br.ReadInt32();
            unk4 = br.ReadInt32();
            sampleRate = br.ReadUInt16();

            unk5 = br.ReadInt16();
            unk6 = br.ReadInt16();
            unk7Reserved = br.ReadInt16();

            offsetAdpcmStateTable = br.ReadInt64();
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}