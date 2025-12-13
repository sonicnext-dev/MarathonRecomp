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

using RageLib.Audio.SoundBank.Hashes;

namespace RageLib.Audio.SoundBank.MultiChannel
{
    internal class SoundWave : ISoundWave
    {
        private Header _header;
        private ChannelInfo _channelInfo;

        public SoundWave(Header header, ChannelInfo channelInfo)
        {
            _header = header;
            _channelInfo = channelInfo;
        }

        #region Implementation of ISoundWave

        public string Name
        {
            get
            {
                return _channelInfo.Name;
            }
        }

        public int NumberOfSamples
        {
            get { return _channelInfo.numSamples16Bit; }
        }

        public int SamplesPerSecond
        {
            get
            {
                return _channelInfo.sampleRate;
            }
        }

        public int BlockSize
        {
            get
            {
                return SoundBankMultiChannel.BlockSize;
            }
        }

        public int BlockCount
        {
            get
            {
                return _header.numBlocks;
            }
        }

        #endregion
    }
}