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
using System.Collections;
using System.Collections.Generic;
using System.IO;
using RageLib.Audio.SoundBank;

namespace RageLib.Audio
{
    public class AudioFile : IEnumerable<AudioWave>, IDisposable
    {
        private SoundBankFile _file;
        public List<AudioWave> AudioWaves { get; private set; }

        internal Stream Stream
        {
            get { return _file.Stream; }
        }

        internal ISoundBank SoundBank
        {
            get { return _file.SoundBank; }
        }

        public int Count
        {
            get { return AudioWaves.Count; }
        }

        public bool IsMultichannel
        {
            get { return _file.IsMultiChannel; }
        }

        public bool SupportsMultichannelExport
        {
            get { return _file.SupportsMultichannelExport; }
        }

        public string Name
        {
            get { return _file.Name; }
        }

        public void Open(string filename)
        {
            _file = new SoundBankFile();
            _file.Open(filename);
            BuildAudioBlocks();
        }

        public void Open(Stream stream)
        {
            _file = new SoundBankFile();
            _file.Open(stream);
            BuildAudioBlocks();
        }


        private void BuildAudioBlocks()
        {
            AudioWaves = new List<AudioWave>();

            int count = _file.SoundBank.WaveCount;
            for (int i = 0; i < count; i++)
            {
                AudioWave wave = new AudioWave(i);
                wave.SoundWave = _file.SoundBank[i];
                AudioWaves.Add(wave);
            }
        }

        #region Implementation of IEnumerable

        public IEnumerator<AudioWave> GetEnumerator()
        {
            return AudioWaves.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

        #endregion

        #region Implementation of IDisposable

        public void Dispose()
        {
            // We keep the Stream object open to play the file so lets dispose it.
            if (_file != null)
            {
                _file.Dispose();
                _file = null;
            }

            AudioWaves.Clear();
            AudioWaves = null;
        }

        #endregion
    }
}