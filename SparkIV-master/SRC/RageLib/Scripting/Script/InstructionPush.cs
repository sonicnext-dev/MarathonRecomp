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
using System.Diagnostics;
using System.Text;

namespace RageLib.Scripting.Script
{
    internal class InstructionPush : Instruction
    {
        private const string PushDirectOp = "PushD";

        private readonly Type _paramType;

        public InstructionPush(Type paramType)
        {
            _paramType = paramType;
        }

        protected override void DecodeInternal(byte[] code, int offset)
        {
            OperandCount = 1;
            if (_paramType == typeof (uint))
            {
                Operands[0] = BitConverter.ToUInt32(code, offset + 1);
            }
            else if (_paramType == typeof (float))
            {
                Operands[0] = BitConverter.ToSingle(code, offset + 1);
            }
            else if (_paramType == typeof (short))
            {
                Operands[0] = BitConverter.ToInt16(code, offset + 1);
            }
            else if (_paramType == typeof (string))
            {
                Operands[0] = Encoding.ASCII.GetString(code, offset + 2, code[offset + 1] - 1);
            }
            else if (_paramType == null)
            {
                // PushD
                Operands[0] = (int) ((uint) OpCode - 80) - 16;
            }
            else
            {
                Debug.Assert(false);
            }
        }

        protected override string GetInstructionTextInternal()
        {
            if (_paramType == null)
            {
                int pushValue = (int) ((uint) OpCode - 80) - 16;
                return PushDirectOp + " " + pushValue;
            }
            else
            {
                return base.GetInstructionTextInternal();
            }
        }
    }
}