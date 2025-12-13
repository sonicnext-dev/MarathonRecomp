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
using RageLib.Scripting.Script;
using File=RageLib.Scripting.Script.File;

namespace RageLib.Scripting.Output
{
    internal class DisassembleOutput : IOutputProvider
    {
        #region IOutputProvider Members

        public void Process(File file, TextWriter writer)
        {
            var decoder = new Decoder(file);

            int length = file.Code.Length;
            int offset = 0;
            while (offset < length)
            {
                Instruction instruction = decoder.Decode(offset);
                writer.WriteLine(instruction.ToString());
                //instruction.ToString();

                offset += instruction.InstructionLength;
            }

            /*
            foreach (uint item in Scruff.Script.Natives.UnknownNatives)
            {
                writer.WriteLine(string.Format("0x{0:x}", item));
            }
             * */
        }

        #endregion
    }
}