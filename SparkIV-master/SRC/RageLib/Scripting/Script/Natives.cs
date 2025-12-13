/**********************************************************************\

 RageLib
 Copyright (C) 2008  Arushan/Aru <oneforaru at gmail.com>

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
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Reflection;
using RageLib.Common;

namespace RageLib.Scripting.Script
{
    public static class Natives
    {
        static Natives()
        {
            using(var s = Assembly.GetExecutingAssembly().GetManifestResourceStream("RageLib.Scripting.Script.NativesList.txt"))
            {
                var sw = new StreamReader(s);
                HashToNative = new Dictionary<uint, string>();

                string item;
                while((item = sw.ReadLine()) != null)
                {
                    uint hash;

                    item = item.Trim();
                    
                    if ( item == "" || item.StartsWith("#") )
                    {
                        continue;
                    }
                    
                    if (item.StartsWith("0x"))
                    {
                        var split = item.Split(new[] {'='}, 2);
                        hash = uint.Parse(split[0].Substring(2), NumberStyles.HexNumber);
                        item = split[1].Trim();
                    }
                    else
                    {
                        hash = Hasher.Hash(item);
                    }

                    try
                    {
                        HashToNative.Add(hash, item);
                    }
                    catch
                    {
                        Debug.WriteLine("Hash collision for: " + item);
                        Debug.Assert(false);
                    }
                }
   
            }
        }

        private static Dictionary<uint, string> HashToNative { get; set; }

        public static string Get(uint hash)
        {
            return HashToNative.ContainsKey(hash) ? HashToNative[hash] : string.Format("0x{0:x}", hash);
        }
    }
}