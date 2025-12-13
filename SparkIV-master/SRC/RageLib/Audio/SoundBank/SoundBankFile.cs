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

namespace RageLib.Audio.SoundBank
{
    class SoundBankFile : IDisposable
    {
        public ISoundBank SoundBank { get; private set; }
        public Stream Stream { get; private set; }
        public bool IsMultiChannel { get; private set; }

        public bool SupportsMultichannelExport
        {
            get { return (SoundBank is IMultichannelSound) && (SoundBank as IMultichannelSound).SupportsMultichannelExport; }
        }

        public string Name
        {
            get { return (SoundBank is IMultichannelSound) ? (SoundBank as IMultichannelSound).CommonFilename : null; }
        }

        public void Open(string filename)
        {
            var stream = new FileStream(filename, FileMode.Open, FileAccess.ReadWrite);
            try
            {
                Open(stream);
            }
            catch
            {
                stream.Close();

                throw;
            }
        }

        public void Open(Stream stream)
        {
            Stream = stream;

            BinaryReader br = new BinaryReader(stream);

            var startPosition = stream.Position;

            // Try loading as multichannel
            try
            {
                stream.Seek(startPosition, SeekOrigin.Begin);
                
                SoundBank = new SoundBankMultiChannel();
                SoundBank.Read(br);
                IsMultiChannel = true;
            }
            catch(SoundBankException)
            {
                SoundBank = null;
            }

            // Failed, so lets try mono
            if (SoundBank == null)
            {
                try
                {
                    stream.Seek(startPosition, SeekOrigin.Begin);
                    
                    SoundBank = new SoundBankMono();
                    SoundBank.Read(br);
                    IsMultiChannel = false;
                }
                catch (SoundBankException)
                {
                    SoundBank = null;
                }
            }

            if (SoundBank == null)
            {
                throw new SoundBankException("Could not load sound bank.");
            }
        }

        #region Implementation of IDisposable

        public void Dispose()
        {
            Stream.Close();
            Stream.Dispose();
            Stream = null;

            SoundBank = null;
        }

        #endregion
    }
}
