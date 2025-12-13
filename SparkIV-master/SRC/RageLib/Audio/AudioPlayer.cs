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
using System.IO;
using RageLib.Audio.SoundBank;
using WaveLib;
using System.Windows.Forms;

namespace RageLib.Audio
{
    class AudioPlayer
    {
        private AudioFile _file;
        private AudioWave _wave;

        private WaveOutPlayer _player;
        private WaveFormat _format;

        private DviAdpcmDecoder.AdpcmState _state;
        private int _lastBlock;
        private bool _looped;
        private byte[] _leftOverBuffer;

        public void Initialize(AudioFile file, AudioWave wave)
        {
            _file = file;
            _wave = wave;

            _format = new WaveFormat(_wave.SamplesPerSecond, 16, 1);

            _lastBlock = -1;
            _looped = false;

            _state = new DviAdpcmDecoder.AdpcmState();
            _leftOverBuffer = null;
        }

        private void Filler(IntPtr data, int size)
        {
            int blockCount = _wave.BlockCount;
            byte[] b = new byte[size];

            if (_file != null && (_looped || _lastBlock < blockCount))
            {
                MemoryStream ms = new MemoryStream();

                if (_leftOverBuffer != null)
                {
                    ms.Write(_leftOverBuffer, 0, _leftOverBuffer.Length);
                }
                
                while (ms.Position < size)
                {
                    _lastBlock++;

                    if (_lastBlock >= blockCount)
                    {
                        if (!_looped)
                        {
                            while(ms.Position < size)
                            {
                                ms.WriteByte(0);
                            }
                            break;
                        }
                        else
                        {
                            _lastBlock = 0;
                            _state = new DviAdpcmDecoder.AdpcmState();
                        }
                    }

                    _file.SoundBank.ExportWaveBlockAsPCM(_wave.Index, _lastBlock, ref _state, _file.Stream, ms);
                }

                int extraData = (int)(ms.Position - size);
                
                ms.Seek(0, SeekOrigin.Begin);
                ms.Read(b, 0, size);

                if (extraData > 0)
                {
                    _leftOverBuffer = new byte[extraData];
                    ms.Read(_leftOverBuffer, 0, extraData);
                }
                else
                {
                    _leftOverBuffer = null;
                }
            }
            else
            {
                for (int i = 0; i < b.Length; i++)
                {
                    b[i] = 0;
                }
            }
            System.Runtime.InteropServices.Marshal.Copy(b, 0, data, size);
        }

        public void Play(bool looped)
        {
            try
            {
                Stop();

                _looped = looped;
                _lastBlock = -1;
                _state = new DviAdpcmDecoder.AdpcmState();
                _player = new WaveOutPlayer(-1, _format, _wave.BlockSize * 4, 3, Filler);
                _leftOverBuffer = null;
            }
            catch
            {
                MessageBox.Show("Audio play error.",
                "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        public void Stop()
        {
            if (_player != null)
            {
                _player.Dispose();
                _player = null;
            }
        }

    }
}
