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

using System.IO;
using File=RageLib.Scripting.Script.File;

namespace RageLib.Scripting.Output
{
    internal class DecompileScruffHeaderOutput : IOutputProvider
    {
        #region IOutputProvider Members

        public void Process(File file, TextWriter writer)
        {
            writer.WriteLine(string.Format("ScriptFlags = 0x{0:x}", file.Header.ScriptFlags));
            writer.WriteLine( string.Format("GlobalsSignature = 0x{0:x}", file.Header.GlobalsSignature) );
            writer.WriteLine();

            if (file.Header.GlobalVarCount > 0)
            {
                writer.WriteLine(string.Format("GlobalsCount = {0}", file.Header.GlobalVarCount));
                writer.WriteLine();
                uint[] globals = file.GlobalVars;
                for(int i=0; i<globals.Length; i++)
                {
                    writer.WriteLine(string.Format("G[{0}] = {1}", i, globals[i]));
                }
                writer.WriteLine();
            }

            writer.WriteLine(string.Format("LocalsCount = {0}", file.Header.LocalVarCount));
            writer.WriteLine();
            uint[] locals = file.LocalVars;
            for (int i = 0; i < locals.Length; i++)
            {
                writer.WriteLine(string.Format("L[{0}] = {1}", i, locals[i]));
            }
            writer.WriteLine();
        }

        #endregion
    }
}