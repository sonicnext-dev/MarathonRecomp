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

namespace RageLib.Audio.SoundBank
{
    interface ISoundBank : IFileAccess
    {
        int WaveCount { get; }
        ISoundWave this[int index] { get; }
        void ExportAsPCM(int index, Stream soundBankStream, Stream outStream);
        int ExportWaveBlockAsPCM(int waveIndex, int blockIndex, ref DviAdpcmDecoder.AdpcmState state, Stream soundBankStream, Stream outStream);
    }
}
