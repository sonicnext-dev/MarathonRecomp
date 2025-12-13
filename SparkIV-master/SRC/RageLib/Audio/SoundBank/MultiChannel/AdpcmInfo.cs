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
using RageLib.Common;

namespace RageLib.Audio.SoundBank.MultiChannel
{
    internal struct AdpcmInfo : IFileAccess
    {
        // adpcm states and info...
        public int numSamples16Bit;
        public short[] unk3;
        public int numStates;
        public DviAdpcmDecoder.AdpcmState[] states;

        public AdpcmInfo(BinaryReader br) : this()
        {
            Read(br);
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            numSamples16Bit = br.ReadInt32();
            unk3 = new short[]
                       {
                           br.ReadInt16(),
                           br.ReadInt16(),
                           br.ReadInt16(),
                           br.ReadInt16(),
                       };
                
            numStates = br.ReadInt32();

            if (numStates > 0)
            {
                states = new DviAdpcmDecoder.AdpcmState[numStates];
                for (int i = 0; i < numStates; i++)
                {
                    states[i].valprev = br.ReadInt16();
                    states[i].index = br.ReadByte();
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