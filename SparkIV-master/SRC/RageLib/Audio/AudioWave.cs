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

using System;
using RageLib.Audio.SoundBank;

namespace RageLib.Audio
{
    public class AudioWave
    {
        internal int Index { get; set; }
        internal ISoundWave SoundWave { get; set; }

        public TimeSpan Length
        {
            get
            {
                var numberOfSeconds = (int) Math.Ceiling((float) NumberOfSamples/SamplesPerSecond);
                return new TimeSpan(0, 0, numberOfSeconds);
            }
        }

        public int NumberOfSamples
        {
            get { return SoundWave.NumberOfSamples; }
        }

        public int SamplesPerSecond
        {
            get { return SoundWave.SamplesPerSecond; }
        }

        public int BlockCount
        {
            get { return SoundWave.BlockCount; }
        }

        public int BlockSize
        {
            get { return SoundWave.BlockSize; }
        }

        public AudioWave(int index)
        {
            Index = index;
        }

        public override string ToString()
        {
            return SoundWave.Name;
        }
    }
}