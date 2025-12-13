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
using System.IO;
using RageLib.Scripting.HLScript;
using RageLib.Scripting.Script;
using File=RageLib.Scripting.Script.File;

namespace RageLib.Scripting.Output
{
    internal class DecompileCFOutput : IOutputProvider
    {
        #region IOutputProvider Members

        public void Process(File file, TextWriter writer)
        {
            var decoder = new Decoder(file);
            var program = new ScriptProgram(decoder);
            var analyzer = new ControlFlowAnalyzer(program);
            analyzer.Analyze();

            foreach (Function function in program.Functions)
            {
                writer.WriteLine(string.Format("{0} {1}(params={2}, vars={3})",
                                               function.ReturnCount > 0 ? "function" : "void", function.Name,
                                               function.ParameterCount, function.VariableCount));
                writer.WriteLine("{");

                ProcessCodePath(writer, function.MainCodePath, "   ");

                writer.WriteLine("}");
                writer.WriteLine();
            }
        }

        #endregion

        private void WriteInstruction(TextWriter writer, HLInstruction instruction, string indent)
        {
            writer.Write(string.Format("{0}{1:x}: ", indent, instruction.Instruction.Offset));
            if (instruction.BranchFunction != null)
            {
                writer.WriteLine(string.Format("{0}()", instruction.BranchFunction.Name));
            }
            else if (instruction.ExitFunction)
            {
                writer.WriteLine("return");
            }
            else if (instruction.Instruction is InstructionNative)
            {
                writer.WriteLine(string.Format("{0}(in={1}, out={2})", instruction.Instruction.Operands[2],
                                               instruction.Instruction.Operands[0], instruction.Instruction.Operands[1]));
            }
            else
            {
                writer.WriteLine(string.Format("{0}", instruction.Instruction.GetInstructionText()));
            }
        }

        private HLInstruction WriteLoopConditional(TextWriter writer, HLInstruction instruction, string indent)
        {
            // Use for loop conditions
            while (instruction != null)
            {
                if (instruction.IsConditionalBranch)
                {
                    break;
                }
                WriteInstruction(writer, instruction, indent);
                instruction = instruction.GetNextInstruction();
            }
            return instruction;
        }

        private void ProcessCodePath(TextWriter writer, CodePath path, string indent)
        {
            HLInstruction instruction = path.GetFirstInstruction();

            while (instruction != null)
            {
                if (instruction.IsConditionalBranch)
                {
                    writer.WriteLine(string.Format("{0}if ({1})", indent,
                                                   instruction.DefaultConditional ? "true" : "false"));
                    writer.WriteLine(indent + "{");
                    ProcessCodePath(writer, instruction.BranchCodesPaths[instruction.DefaultConditional],
                                    indent + "    ");
                    writer.WriteLine(indent + "}");

                    if (instruction.BranchCodesPaths.ContainsKey(!instruction.DefaultConditional))
                    {
                        CodePath elsePath = instruction.BranchCodesPaths[!instruction.DefaultConditional];

                        if (elsePath.StartInstruction != null)
                        {
                            writer.WriteLine(indent + "else");
                            writer.WriteLine(indent + "{");
                            ProcessCodePath(writer, elsePath,
                                            indent + "    ");
                            writer.WriteLine(indent + "}");
                        }
                    }
                }
                else if (instruction.IsSwitchBranch)
                {
                    // Do switch cases ever fall through?? I'm assuming they don't here!

                    writer.WriteLine(indent + "switch");
                    writer.WriteLine(indent + "{");

                    // Keep track of code paths are have already outputted to keep
                    // track of offsets that lead to the same codepath
                    var swDonePaths = new List<CodePath>();

                    foreach (var item in instruction.BranchCodesPaths)
                    {
                        if (swDonePaths.Contains(item.Value))
                        {
                            continue;
                        }

                        foreach (var item2 in instruction.BranchCodesPaths)
                        {
                            // O(n^2) loop here, there's probably a better way to optimize it

                            if (item2.Value == item.Value)
                            {
                                if (item2.Key.GetType() == typeof (int))
                                {
                                    writer.WriteLine(string.Format("{0}    case {1}:", indent, item2.Key));
                                }
                                else
                                {
                                    writer.WriteLine(string.Format("{0}    default:", indent, item2.Key));
                                }
                            }
                        }

                        writer.WriteLine(indent + "    {");
                        ProcessCodePath(writer, item.Value, indent + "        ");
                        if (item.Value.EndInstruction == null || !item.Value.EndInstruction.ExitFunction)
                        {
                            writer.WriteLine(indent + "        break");
                        }
                        writer.WriteLine(indent + "    }");

                        swDonePaths.Add(item.Value);
                    }

                    writer.WriteLine(indent + "}");
                }
                else if (instruction.LoopCodePath != null)
                {
                    // First of a loop instruction (hopefully, someday, this will be extracted out by the ProgramAnalyzer)
                    // Can we ever break out of a loop? I assume we can't here!

                    writer.WriteLine(indent + "while");
                    writer.WriteLine(indent + "(");
                    instruction = WriteLoopConditional(writer, instruction, indent + "    ");
                    writer.WriteLine(indent + ")");
                    writer.WriteLine(indent + "{");
                    ProcessCodePath(writer, instruction.BranchCodesPaths[true], indent + "    ");
                    writer.WriteLine(indent + "}");
                }
                else
                {
                    WriteInstruction(writer, instruction, indent);
                }

                instruction = instruction.GetNextInstruction();
            }
        }
    }
}