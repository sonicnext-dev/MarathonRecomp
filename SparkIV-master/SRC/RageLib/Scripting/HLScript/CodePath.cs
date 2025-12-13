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

namespace RageLib.Scripting.HLScript
{
    internal class CodePath
    {
        public CodePath()
        {
            InstructionMap = new Dictionary<int, HLInstruction>();
        }

        public Function ParentFunction { get; set; }
        public CodePath ParentCodePath { get; set; }

        public int StartOffset { get; set; }
        public int EndOffset { get; set; }
        public string Name { get; set; }

        public HLInstruction ParentExitInstruction { get; set; } // The instruction in Parent that we exit from
        public HLInstruction ParentEntryInstruction { get; set; } // The instruction in Parent that we re-enter into (if null, exit parent path)

        public HLInstruction StartInstruction { get; set; } // The instruction we start executing this code path from
        public HLInstruction EndInstruction { get; set; } // The very last instruction in this code path before we re-enter into parent

        public HLInstruction ParentEntryTargetInstruction
        {
            get
            {
                if (ParentEntryInstruction == null)
                {
                    if (ParentCodePath == null)
                    {
                        // This can can only mean that we are in the function root, but this case is not really possible, is it?
                        return null;
                    }
                    else
                    {
                        return ParentCodePath.ParentEntryTargetInstruction;
                    }
                }
                else
                {
                    return ParentEntryInstruction;
                }
            }
        }

        public Dictionary<int, HLInstruction> InstructionMap { get; private set; }

        public HLInstruction GetInstruction(int offset)
        {
            if (InstructionMap.ContainsKey(offset))
            {
                return InstructionMap[offset];
            }
            return null;
        }

        public bool IsInstructionOffsetInPathTree(int offset)
        {
            bool found = false;

            if (InstructionMap.ContainsKey(offset))
            {
                found = true;
            }
            else if (ParentCodePath != null)
            {
                found = ParentCodePath.IsInstructionOffsetInPathTree(offset);
            }

            return found;
        }

        public HLInstruction GetFirstInstruction()
        {
            return StartInstruction;
        }

        public override string ToString()
        {
            return Name;
        }
    }
}