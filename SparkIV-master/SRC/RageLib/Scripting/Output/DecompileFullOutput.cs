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
using System.Text;
using RageLib.Scripting.HLScript;
using RageLib.Scripting.Script;
using Decoder=RageLib.Scripting.Script.Decoder;
using File=RageLib.Scripting.Script.File;

namespace RageLib.Scripting.Output
{
    internal class DecompileFullOutput : IOutputProvider
    {
        private readonly bool _annotate;

        private bool _nextIfIsAnElseIf = false;

        public DecompileFullOutput(bool annotate)
        {
            _annotate = annotate;
        }

        #region IOutputProvider Members

        public void Process(File file, TextWriter writer)
        {
            var decoder = new Decoder(file);
            var program = new ScriptProgram(decoder);
            var analyzer = new ControlFlowAnalyzer(program);
            analyzer.Analyze();

            var stackAnalyzer = new StackUseAnalyzer(program);
            stackAnalyzer.Analyze();

            foreach (Function function in program.Functions)
            {
                var sb = new StringBuilder();
                for (int i = 0; i < function.ParameterCount; i++)
                {
                    if (i != 0)
                    {
                        sb.Append(", ");
                    }
                    sb.Append("var");
                    sb.Append(i);
                }

                writer.WriteLine(string.Format("{0} {1}({2})", function.ReturnCount > 0 ? "function" : "void",
                                               function.Name, sb));
                writer.WriteLine("{");

                if (function.VariableCount > 2)
                {
                    writer.Write("   auto ");
                    for (int i = 2; i < function.VariableCount; i++)
                    {
                        if (i != 2)
                        {
                            writer.Write(", ");
                        }
                        writer.Write("var" + (i + function.ParameterCount));
                    }
                    writer.WriteLine(";");
                }

                if (function.TemporaryCount > 0)
                {
                    writer.Write("   auto ");
                    for (int i = 0; i < function.TemporaryCount; i++)
                    {
                        if (i != 0)
                        {
                            writer.Write(", ");
                        }
                        writer.Write("temp" + i);
                    }
                    writer.WriteLine(";");
                }

                if (function.TemporaryCount > 0 || function.VariableCount > 2)
                {
                    writer.WriteLine();
                }

                ProcessCodePath(writer, function.MainCodePath, "   ");

                writer.WriteLine("}");
                writer.WriteLine();
            }
        }

        #endregion

        private void WriteInstruction(TextWriter writer, HLInstruction instruction, string indent)
        {
            if (instruction.ProcessedStackValue != null)
            {
                if (instruction.ProcessedStackValue is ProcessedStackValueGroup)
                {
                    var group = instruction.ProcessedStackValue as ProcessedStackValueGroup;
                    foreach (StackValue value in group.Values)
                    {
                        writer.WriteLine(string.Format("{0}{1};", indent, value));
                    }
                }
                else
                {
                    Function parentFunction = instruction.ParentCodePath.ParentFunction;
                    if (parentFunction.ReturnCount == 0 && 
                        instruction.ExitFunction && 
                        instruction == parentFunction.MainCodePath.EndInstruction)
                    {
                        // Don't write the default return unless there's a return value!
                        //writer.WriteLine(string.Format("{0}// {1};", indent, instruction.ProcessedStackValue));
                    }
                    else
                    {
                        writer.WriteLine(string.Format("{0}{1};", indent, instruction.ProcessedStackValue));
                    }
                }
            }
        }

        private bool IsCodePathAnIfPath(CodePath path)
        {
            HLInstruction instruction = path.GetFirstInstruction();
            bool foundOneIf = false;
            while (instruction != null)
            {
                if (instruction.IsConditionalBranch && instruction.ProcessedStackValue != null)
                {
                    foundOneIf = true;
                }
                else
                {
                    if (instruction.ProcessedStackValue != null)
                    {
                        return false;
                    }
                }

                instruction = instruction.GetNextInstruction();
            }

            return foundOneIf;
        }
        
        private void Annotate(TextWriter writer, string indent, HLInstruction instruction)
        {
            if (_annotate)
            {
                writer.WriteLine(string.Format("{0}// {1}", indent, instruction.Instruction));
            }
        }

        private void ProcessCodePath(TextWriter writer, CodePath path, string indent)
        {
            HLInstruction instruction = path.GetFirstInstruction();

            while (instruction != null)
            {
                Annotate(writer, indent, instruction);

                if (instruction.UnconditionalBranch)
                {
                    // Not used
                }
                else if (instruction.IsConditionalBranch)
                {
                    if (_nextIfIsAnElseIf)
                    {
                        writer.Write(string.Format("{0}else if", indent));
                        _nextIfIsAnElseIf = false;
                    }
                    else
                    {
                        writer.Write(string.Format("{0}if", indent));
                    }
                    writer.WriteLine(string.Format(" ({0})", instruction.ProcessedStackValue));
                    writer.WriteLine(indent + "{");
                    ProcessCodePath(writer, instruction.BranchCodesPaths[instruction.DefaultConditional],
                                    indent + "    ");
                    writer.WriteLine(indent + "}");

                    if (instruction.BranchCodesPaths.ContainsKey(!instruction.DefaultConditional))
                    {
                        CodePath elsePath = instruction.BranchCodesPaths[!instruction.DefaultConditional];
                        if (elsePath.StartInstruction != null)
                        {
                            if (IsCodePathAnIfPath(elsePath))
                            {
                                _nextIfIsAnElseIf = true;
                                ProcessCodePath(writer, elsePath, indent);
                            }
                            else
                            {
                                writer.WriteLine(indent + "else");
                                writer.WriteLine(indent + "{");
                                ProcessCodePath(writer, elsePath, indent + "    ");
                                writer.WriteLine(indent + "}");
                            }
                        }
                    }
                }
                else if (instruction.IsSwitchBranch)
                {
                    // Do switch cases ever fall through?? I'm assuming they don't here!

                    writer.WriteLine(indent + string.Format("switch ({0})", instruction.ProcessedStackValue));
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
                                    writer.WriteLine(string.Format("{0}    case {1}:", indent, LiteralFormatter.FormatInteger((int)item2.Key)));
                                }
                                else
                                {
                                    writer.WriteLine(string.Format("{0}    default:", indent));
                                }
                            }
                        }

                        writer.WriteLine(indent + "    {");
                        ProcessCodePath(writer, item.Value, indent + "        ");
                        if (item.Value.EndInstruction == null || !item.Value.EndInstruction.ExitFunction)
                        {
                            writer.WriteLine(indent + "        break;");
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

                    while (!instruction.IsConditionalBranch)
                    {
                        instruction = instruction.NextInstruction;
                        Annotate(writer, indent, instruction);
                    }

                    writer.WriteLine(indent + string.Format("while ({0})", instruction.ProcessedStackValue));
                    writer.WriteLine(indent + "{");
                    ProcessCodePath(writer, instruction.BranchCodesPaths[true], indent + "    ");
                    writer.WriteLine(indent + "}");
                }
                else
                {
                    WriteInstruction(writer, instruction, indent);
                }

                instruction = instruction.NextInstruction;
            }
        }
    }
}