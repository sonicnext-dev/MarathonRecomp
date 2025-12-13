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
using RageLib.Scripting.HLScript;
using RageLib.Scripting.Script;
using File=RageLib.Scripting.Script.File;

namespace RageLib.Scripting.Output
{
    internal class CodePathOutput : IOutputProvider
    {
        #region IOutputProvider Members

        public void Process(File file, TextWriter writer)
        {
            var decoder = new Decoder(file);
            var program = new ScriptProgram(decoder);
            var analyzer = new ControlFlowAnalyzer(program);
            analyzer.Analyze();

            // Dump the code paths
            foreach (Function function in program.Functions)
            {
                writer.WriteLine("function " + function.Name);
                foreach (CodePath path in function.CodePaths)
                {
                    writer.WriteLine("    " + path.Name + ":");
                    writer.WriteLine(string.Format("        0x{0:x} --> 0x{1:x}", path.StartOffset, path.EndOffset));
                    if (path.ParentCodePath != null)
                    {
                        writer.WriteLine("        parent: {0}, exit: 0x{1:x}, reentry: 0x{2:x}",
                                         path.ParentCodePath.Name, path.ParentExitInstruction.Instruction.Offset,
                                         path.ParentEntryTargetInstruction.Instruction.Offset);
                    }
                }
                writer.WriteLine();
            }
        }

        #endregion
    }
}