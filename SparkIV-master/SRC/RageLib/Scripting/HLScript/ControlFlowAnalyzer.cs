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

using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using RageLib.Scripting.Script;

namespace RageLib.Scripting.HLScript
{
    internal class ControlFlowAnalyzer
    {
        public ControlFlowAnalyzer(ScriptProgram program)
        {
            Program = program;
            FunctionTargets = new Dictionary<int, Function>();
        }

        private ScriptProgram Program { get; set; }

        private Dictionary<int, Function> FunctionTargets { get; set; }

        private Decoder Decoder
        {
            get { return Program.Decoder; }
        }

        private List<Function> Functions
        {
            get { return Program.Functions; }
        }

        public void Analyze()
        {
            // Generate all the functions
            GenerateFunctions();

            // Analyze the functions into a series of HL instructions
            AnalyzeFunctions();
        }

        private void GenerateFunctions()
        {
            int offset = 0;

            while (offset < Decoder.File.Header.CodeSize)
            {
                Instruction instruction = Decoder.Decode(offset);
                offset += instruction.InstructionLength;

                if (instruction is InstructionFnBegin)
                {
                    var function = new Function();
                    function.Instruction = instruction;
                    function.StartOffset = instruction.Offset;
                    function.Name = (function.StartOffset == 0)
                                        ? "main"
                                        : string.Format("sub_{0:x}", instruction.Offset);
                    FunctionTargets.Add(instruction.Offset, function);
                    Functions.Add(function);
                }
            }
        }

        private static CodePath FindCommonPathAncestor(CodePath path1, CodePath path2)
        {
            var paths = new Dictionary<CodePath, object>();

            while(path1 != null)
            {
                paths.Add(path1, null);
                path1 = path1.ParentCodePath;
            }

            while(path2 != null)
            {
                if (paths.ContainsKey(path2))
                {
                    return path2;
                }
                path2 = path2.ParentCodePath;
            }

            return null;
        }

        private static CodePath CreateCodePath(string identifier, int startOffset, CodePath parentPath)
        {
            CodePath path = parentPath.ParentFunction.CreateCodePath(string.Format("{0}_{1:x}", identifier, startOffset));
            path.ParentCodePath = parentPath;
            path.StartOffset = startOffset;
            return path;
        }

        /// <summary>
        /// Extracts a sequence of instructions into a new sub codepath
        /// </summary>
        /// <param name="identifier"></param>
        /// <param name="parentPath"></param>
        /// <param name="parentExitInstruction"></param>
        /// <param name="startInstruction">inclusive</param>
        /// <param name="endInstruction">exclusive (this is where our codepath returns to)</param>
        /// <returns></returns>
        private static CodePath ExtractPath(string identifier, CodePath parentPath, HLInstruction parentExitInstruction, HLInstruction startInstruction,
                                     HLInstruction endInstruction)
        {
            CodePath toPath = CreateCodePath(identifier, startInstruction.Instruction.Offset, parentPath);
            toPath.ParentCodePath = parentPath;
            toPath.ParentExitInstruction = parentExitInstruction;

            HLInstruction hlInstruction = startInstruction;

            while (true)
            {
                parentPath.InstructionMap.Remove(hlInstruction.Instruction.Offset);
                hlInstruction.ParentCodePath = toPath;
                toPath.InstructionMap.Add(hlInstruction.Instruction.Offset, hlInstruction);

                if (hlInstruction.NextInstruction == null || hlInstruction.NextInstruction == endInstruction)
                {
                    hlInstruction.NextInstruction = null;
                    break;
                }

                hlInstruction = hlInstruction.NextInstruction;
            }

            if (startInstruction.PreviousInstruction != null)
            {
                startInstruction.PreviousInstruction.NextInstruction = endInstruction;
            }
            else
            {
                parentPath.StartInstruction = endInstruction;
                if (endInstruction != null)
                {
                    parentPath.StartOffset = endInstruction.Instruction.Offset;
                }
            }

            if (endInstruction != null)
            {
                endInstruction.PreviousInstruction = startInstruction.PreviousInstruction;
            }
            if (endInstruction == null)
            {
                parentPath.EndInstruction = startInstruction.PreviousInstruction;
                if (parentPath.EndInstruction != null)
                {
                    parentPath.EndOffset = parentPath.EndInstruction.Instruction.Offset +
                                           parentPath.EndInstruction.Instruction.InstructionLength;
                }
                else
                {
                    parentPath.EndOffset = parentPath.StartOffset;
                }
            }

            startInstruction.PreviousInstruction = null;

            toPath.StartInstruction = startInstruction;
            toPath.EndInstruction = hlInstruction;

            toPath.EndOffset = hlInstruction.Instruction.Offset + hlInstruction.Instruction.InstructionLength;

            if (endInstruction != null && parentPath == endInstruction.ParentCodePath)
            {
                toPath.ParentEntryInstruction = endInstruction;
            }
            else
            {
                toPath.ParentEntryInstruction = null;
            }

            return toPath;
        }

        private static void MergePath(CodePath targetPath, CodePath newPath)
        {
            HLInstruction hlInstruction = newPath.StartInstruction;

            if (targetPath.EndInstruction == null)
            {
                targetPath.StartInstruction = hlInstruction;
                hlInstruction.PreviousInstruction = null;
            }
            else
            {
                targetPath.EndInstruction.NextInstruction = hlInstruction;
                hlInstruction.PreviousInstruction = targetPath.EndInstruction;
            }

            while (true)
            {
                newPath.InstructionMap.Remove(hlInstruction.Instruction.Offset);
                hlInstruction.ParentCodePath = targetPath;
                targetPath.InstructionMap.Add(hlInstruction.Instruction.Offset, hlInstruction);

                if (hlInstruction.NextInstruction == null)
                {
                    break;
                }

                hlInstruction = hlInstruction.NextInstruction;
            }

            targetPath.EndInstruction = hlInstruction;
            targetPath.EndOffset = hlInstruction.Instruction.Offset + hlInstruction.Instruction.InstructionLength;

            newPath.ParentFunction.CodePaths.Remove(newPath);
        }

        /// <summary>
        /// Dumps a codepath to debug
        /// </summary>
        /// <param name="path"></param>
        private static void DumpCodePath(CodePath path)
        {
            HLInstruction instruction = path.StartInstruction;
            while (instruction != null)
            {
                Debug.WriteLine(instruction.Instruction.ToString());
                if (instruction.BranchCodesPaths != null && instruction.BranchCodesPaths.Count > 0)
                {
                    foreach (var item in instruction.BranchCodesPaths)
                    {
                        Debug.WriteLine("    " + item.Key + " --> " + item.Value.Name);
                    }
                }
                instruction = instruction.NextInstruction;
            }
        }

        private void AnalyzeFunctions()
        {
            foreach (Function function in Functions)
            {
                int offset = function.StartOffset;
                Instruction instruction = Decoder.Decode(offset);

                Debug.Assert(instruction is InstructionFnBegin);

                var fnBegin = instruction as InstructionFnBegin;
                if (fnBegin != null)
                {
                    function.ParameterCount = fnBegin.ParameterCount;
                    function.VariableCount = fnBegin.VarCount;
                    
                    function.MainCodePath.StartOffset = function.Instruction.Offset + function.Instruction.InstructionLength;

                    AnalyzeCodePath(function.MainCodePath);
                }
            }
        }

        private void AnalyzeSwitchBranch(HLInstruction branchInstruction)
        {
            var sw = branchInstruction.Instruction as InstructionSwitch;

            CodePath parentPath = branchInstruction.ParentCodePath;

            var swMap = new Dictionary<int, CodePath>();
            bool allWereReturns = true;

            HLInstruction switchExitInstruction = branchInstruction.NextInstruction.NextInstruction;

            foreach (var item in sw.SwitchTable)
            {
                if (swMap.ContainsKey(item.Value))
                {
                    // Deal with multiple values leading to the same code path
                    branchInstruction.BranchCodesPaths.Add(item.Key, swMap[item.Value]);
                }
                else
                {
                    CodePath branchCodePath = CreateCodePath("sw_case_" + item.Key, item.Value, parentPath);
                    branchCodePath.ParentExitInstruction = branchInstruction;

                    branchInstruction.BranchCodesPaths.Add(item.Key, branchCodePath);
                    swMap.Add(item.Value, branchCodePath);

                    AnalyzeCodePath(branchCodePath);

                    if (branchCodePath.ParentEntryInstruction != null)
                    {
                        allWereReturns = false;
                        switchExitInstruction = branchCodePath.ParentEntryInstruction;
                    }
                }
            }

            if (allWereReturns)
            {
                switchExitInstruction = null;
            }

            if (switchExitInstruction != branchInstruction.NextInstruction.NextInstruction)
            {
                // Crappy... got a default case statement that we need to extract

                CodePath branchDefaultCodePath = ExtractPath("sw_case_default", parentPath, branchInstruction, branchInstruction.NextInstruction, switchExitInstruction);

                branchInstruction.BranchCodesPaths.Add(new object(), branchDefaultCodePath);
            }
        }

        private void AnalyzeConditionalBranch(HLInstruction branchInstruction)
        {
            int branchOffset = branchInstruction.Instruction.Offset +
                   branchInstruction.Instruction.InstructionLength;

            CodePath parentPath = branchInstruction.ParentCodePath;

            CodePath branchCodePath = CreateCodePath("if", branchOffset, parentPath);
            branchCodePath.ParentExitInstruction = branchInstruction;

            bool condition = branchInstruction.Instruction.OpCode == OpCode.JumpFalse ? true : false;
            branchInstruction.BranchCodesPaths.Add(condition, branchCodePath);

            branchInstruction.DefaultConditional = condition;

            AnalyzeCodePath(branchCodePath);

            if (branchInstruction.NextInstruction != null &&
                branchCodePath.ParentEntryTargetInstruction != branchInstruction.NextInstruction)
            {
                // Well, shit.. we got an else block.

                CodePath branchElseCodePath = ExtractPath("if_else", branchInstruction.ParentCodePath, branchInstruction,
                                                          branchInstruction.NextInstruction,
                                                          branchCodePath.ParentEntryInstruction);

                branchInstruction.BranchCodesPaths.Add(!condition, branchElseCodePath);
            }

        }

        private void AnalyzeBranchInstructions(IEnumerable<HLInstruction> branchInstructions)
        {
            foreach (HLInstruction branchInstruction in branchInstructions)
            {
                if (branchInstruction.IsConditionalBranch)
                {
                    AnalyzeConditionalBranch(branchInstruction);
                }
                else if (branchInstruction.IsSwitchBranch)
                {
                    AnalyzeSwitchBranch(branchInstruction);
                }
                else
                {
                    Debug.Assert(false);
                }
            }
        }

        private void AnalyzeCodePath(CodePath path)
        {
            HLInstruction previousInstruction = null;
            var branchInstructions = new List<HLInstruction>();

            int offset = path.StartOffset;

            while (true)
            {
                if (path.ParentCodePath != null) // non main
                {
                    CodePath parentPath = path.ParentFunction.DetermineCodePathFromOffset(offset);
                    if (parentPath != null)
                    {
                        if (!path.IsInstructionOffsetInPathTree(offset))
                        {
                            // Hmm.. so the target offset is not the current tree... which is bad
                            // This can only mean a bad jump assumption before. We must somehow extract
                            // The instructions from this existing path and merge it with our parent

                            HLInstruction startInstruction = parentPath.InstructionMap[offset];
                            CodePath newPath = ExtractPath(path.ParentCodePath.Name + "_temp", parentPath,
                                                           startInstruction.PreviousInstruction, startInstruction, null);

                            // Now we can merge it with this path's parent

                            CodePath targetCodePath = FindCommonPathAncestor(path, parentPath);
                            MergePath(targetCodePath, newPath);

                            if (parentPath.ParentCodePath == targetCodePath)
                            {
                                parentPath.ParentEntryInstruction = startInstruction;
                            }
                            else
                            {
                                parentPath.ParentEntryInstruction = null;
                            }

                            // Set some variables so we know how to break out of here.

                            parentPath = targetCodePath;
                        }

                        // Okay, break out of there buddy!

                        if (parentPath == path.ParentCodePath)
                        {
                            path.ParentEntryInstruction = path.ParentCodePath.GetInstruction(offset);
                        }
                        else
                        {
                            // null = we reached the end of the code path for the parent as well!
                            path.ParentEntryInstruction = null;
                        }
                        break;
                    }

                }

                Instruction instruction = Decoder.Decode(offset);
                offset += instruction.InstructionLength;

                var hlInstruction = new HLInstruction(instruction);

                if (instruction is InstructionFnEnd)
                {
                    if (path.ParentCodePath == null)
                    {
                        // This means this is in the function root

                        path.ParentFunction.EndOffset = instruction.Offset + instruction.InstructionLength;
                        path.ParentFunction.ReturnCount = (instruction as InstructionFnEnd).ReturnCount;

                        hlInstruction.ExitFunction = true;
                    }
                    else
                    {
                        if (instruction.Offset == path.ParentFunction.EndOffset)
                        {
                            // Non root code path reached end of function
                            path.ParentEntryInstruction = null;

                            hlInstruction.ExitFunction = true;
                        }
                        else
                        {
                            // Hmm, this must be a exit function invocation
                            // We'll add it, and exit out of the loop later in the code

                            hlInstruction.ExitFunction = true;
                        }
                    }
                }
                else if (instruction is InstructionBranch)
                {
                    if (instruction.OpCode == OpCode.Call)
                    {
                        hlInstruction.BranchFunction = FunctionTargets[instruction.BranchOffset];
                    }
                    else if (instruction.OpCode == OpCode.Jump)
                    {
                        if (path.ParentCodePath != null && instruction.BranchOffset < instruction.Offset &&
                            path.ParentCodePath.GetInstruction(instruction.BranchOffset) != null)
                        {
                            // Loop!
                            hlInstruction.BranchLoopInstruction =
                                path.ParentCodePath.GetInstruction(instruction.BranchOffset);
                            hlInstruction.BranchLoopInstruction.LoopCodePath = path;

                            // The next instruction should break us out of this codepath...

                            hlInstruction.UnconditionalBranch = true;
                        }
                        else
                        {
                            // Okay, we'll just be naive for now and follow the jump
                            // It is *possible* that this jump might be for an "else" or "case default" block, in which case, we are screwed.
                            // But we will deal with this while we deal with branches
                            hlInstruction.UnconditionalBranch = true;
                            offset = instruction.BranchOffset;
                        }
                    }
                    else
                    {
                        // We got a conditional branch instruction
                        hlInstruction.IsConditionalBranch = true;
                        branchInstructions.Add(hlInstruction);
                        offset = instruction.BranchOffset;
                    }
                }
                else if (instruction is InstructionSwitch)
                {
                    hlInstruction.IsSwitchBranch = true;
                    branchInstructions.Add(hlInstruction);
                }

                hlInstruction.PreviousInstruction = previousInstruction;
                if (previousInstruction == null)
                {
                    path.StartInstruction = hlInstruction;
                }
                else
                {
                    previousInstruction.NextInstruction = hlInstruction;
                }
                previousInstruction = hlInstruction;

                hlInstruction.ParentCodePath = path;
                path.InstructionMap.Add(instruction.Offset, hlInstruction);

                if (hlInstruction.ExitFunction)
                {
                    break; // out of while loop
                }
            }

            path.EndInstruction = previousInstruction;
            path.EndOffset = offset;

            AnalyzeBranchInstructions(branchInstructions);
        }
    }
}
