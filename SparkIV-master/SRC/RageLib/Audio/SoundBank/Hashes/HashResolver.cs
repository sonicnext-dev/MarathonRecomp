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

using System.Collections.Generic;
using System.IO;
using System.Reflection;
using RageLib.Common;

namespace RageLib.Audio.SoundBank.Hashes
{
    internal static class HashResolver
    {
        private static readonly Dictionary<uint, string> _knownNames;

        static HashResolver()
        {
            _knownNames = new Dictionary<uint, string>();

            LoadFromResource("Names.txt");
        }

        private static void LoadFromResource(string filename)
        {
            using (var s = Assembly.GetExecutingAssembly().GetManifestResourceStream(typeof(HashResolver), filename))
            {
                var sw = new StreamReader(s);

                string name;
                while ((name = sw.ReadLine()) != null)
                {
                    uint hash = Hasher.Hash(name);
                    if (!_knownNames.ContainsKey(hash))
                    {
                        _knownNames.Add(hash, name);
                    }
                }
            }
        }

        public static string Resolve(uint hash)
        {
            if (_knownNames.ContainsKey(hash))
            {
                return _knownNames[hash];
            }
            else
            {
                return string.Format("0x{0:x}", hash);
            }
        }
    }
}