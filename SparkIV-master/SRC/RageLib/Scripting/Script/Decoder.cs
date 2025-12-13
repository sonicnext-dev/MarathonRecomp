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

using System.Diagnostics;

namespace RageLib.Scripting.Script
{
    internal class Decoder
    {
        public Decoder(File file)
        {
            File = file;
        }

        public File File { get; set; }

        public Instruction Decode(int offset)
        {
            Instruction instruction = null;

            var opcode = (OpCode) File.Code[offset];

            switch (opcode)
            {
                case OpCode.PushS:
                    instruction = new InstructionPush(typeof (short));
                    break;
                case OpCode.Push:
                    instruction = new InstructionPush(typeof (uint));
                    break;
                case OpCode.PushF:
                    instruction = new InstructionPush(typeof (float));
                    break;
                case OpCode.PushString:
                    instruction = new InstructionPush(typeof (string));
                    break;
                case OpCode.Jump:
                case OpCode.JumpFalse:
                case OpCode.JumpTrue:
                case OpCode.Call:
                    instruction = new InstructionBranch();
                    break;
                case OpCode.CallNative:
                    instruction = new InstructionNative();
                    break;
                case OpCode.FnBegin:
                    instruction = new InstructionFnBegin();
                    break;
                case OpCode.FnEnd:
                    instruction = new InstructionFnEnd();
                    break;
                case OpCode.Switch:
                    instruction = new InstructionSwitch();
                    break;
                default:
                    if ((int) opcode >= 80)
                    {
                        instruction = new InstructionPush(null);
                    }
                    else if ((int) opcode <= 78)
                    {
                        instruction = new Instruction();
                    }
                    else
                    {
                        Debug.Assert(false);
                    }
                    break;
            }

            instruction.Decode(File.Code, offset);

            return instruction;
        }
    }
}