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

using System;

namespace RageLib.Scripting.Script
{
    internal class InstructionBranch : Instruction
    {
        protected override void DecodeInternal(byte[] code, int offset)
        {
            IsConditionalBranch = (OpCode == OpCode.JumpFalse || OpCode == OpCode.JumpTrue);
            BranchOffset = BitConverter.ToInt32(code, offset + 1);
        }

        protected override string GetInstructionTextInternal()
        {
            return string.Format("{0} @{1:x}", OpCode, BranchOffset);
        }
    }
}