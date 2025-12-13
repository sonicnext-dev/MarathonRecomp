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

namespace RageLib.Audio.WaveFile
{
    [Flags]
    internal enum ChannelMask
    {
        Invalid = 0x0,
        SpeakerFrontLeft = 0x1,
        SpeakerFrontRight = 0x2,
        SpeakerFrontCenter = 0x4,
        SpeakerLowFrequency = 0x8,
        SpeakerBackLeft = 0x10,
        SpeakerBackRight = 0x20,
        SpeakerFrontLeftOfCenter = 0x40,
        SpeakerFrontRightOfCenter = 0x80,
        SpeakerBackCenter = 0x100,
        SpeakerSideLeft = 0x200,
        SpeakerSideRight = 0x400,
        SpeakerTopCenter = 0x800,
        SpeakerTopFrontLeft = 0x1000,
        SpeakerTopFrontCenter = 0x2000,
        SpeakerTopFrontRight = 0x4000,
        SpeakerTopBackLeft = 0x8000,
        SpeakerTopBackCenter = 0x10000,
        SpeakerTopBackRight = 0x20000,
    }
}