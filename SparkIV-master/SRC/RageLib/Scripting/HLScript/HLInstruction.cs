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
using RageLib.Scripting.Script;

namespace RageLib.Scripting.HLScript
{
    internal class HLInstruction
    {
        public HLInstruction(Instruction instruction)
        {
            Instruction = instruction;

            if (instruction is InstructionBranch || instruction is InstructionSwitch)
            {
                BranchCodesPaths = new Dictionary<object, CodePath>();
            }
        }

        public CodePath ParentCodePath { get; set; }

        public Instruction Instruction { get; set; }

        public HLInstruction PreviousInstruction { get; set; }
        public HLInstruction NextInstruction { get; set; }

        public Dictionary<object, CodePath> BranchCodesPaths { get; private set; }

        public bool ExitFunction { get; set; }
        public bool UnconditionalBranch { get; set; }

        public CodePath LoopCodePath { get; set; }
        public HLInstruction BranchLoopInstruction { get; set; }

        public bool IsConditionalBranch { get; set; }
        public bool DefaultConditional { get; set; }

        public bool IsSwitchBranch { get; set; }

        public Function BranchFunction { get; set; }

        public StackValue ProcessedStackValue { get; set; }

        public HLInstruction GetNextInstruction()
        {
            HLInstruction instruction = NextInstruction;
            if (instruction != null && instruction.UnconditionalBranch)
            {
                instruction = instruction.NextInstruction;
            }
            return instruction;
        }

        public override string ToString()
        {
            return Instruction.ToString();
        }
    }
}