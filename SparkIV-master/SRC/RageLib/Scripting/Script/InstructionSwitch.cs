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
using System.Collections.Generic;
using System.Text;

namespace RageLib.Scripting.Script
{
    internal class InstructionSwitch : Instruction
    {
        public Dictionary<int, int> SwitchTable { get; private set; }

        protected override void DecodeInternal(byte[] code, int offset)
        {
            SwitchTable = new Dictionary<int, int>();

            int switchCount = code[offset + 1];

            for (int i = 0; i < switchCount; i++)
            {
                int index = BitConverter.ToInt32(code, offset + 2 + i*8);
                int jump = BitConverter.ToInt32(code, offset + 2 + i*8 + 4);

                SwitchTable.Add(index, jump);
            }
        }

        protected override string GetInstructionTextInternal()
        {
            var str = new StringBuilder();

            str.Append(OpCode.ToString());
            str.Append(" ");

            bool first = true;
            foreach (var item in SwitchTable)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    str.Append(", ");
                }

                str.Append(LiteralFormatter.FormatInteger(item.Key));
                str.Append("->@");
                str.Append(string.Format("{0:x}", item.Value));
            }

            return str.ToString();
        }
    }
}