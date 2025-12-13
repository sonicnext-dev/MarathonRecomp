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

namespace RageLib.Scripting.Script
{
    internal class InstructionFnEnd : Instruction
    {
        public int ParameterCount
        {
            get { return (int) Operands[0]; }
        }

        public int ReturnCount
        {
            get { return (int) Operands[1]; }
        }

        protected override void DecodeInternal(byte[] code, int offset)
        {
            OperandCount = 2;
            Operands[0] = (int) code[offset + 1];
            Operands[1] = (int) code[offset + 2];
        }

        protected override string GetOperandName(int index)
        {
            if (index == 0) return "params";
            if (index == 1) return "return";
            return null;
        }
    }
}