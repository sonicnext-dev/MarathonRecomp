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
    internal class Instruction
    {
        public object[] Operands { get; protected set; }
        public OpCode OpCode { get; protected set; }

        public int Offset { get; protected set; }

        public int OperandCount
        {
            get { return Operands.Length; }
            set { Operands = new object[value]; }
        }

        public int InstructionLength { get; protected set; }
        public bool IsConditionalBranch { get; protected set; }
        public int BranchOffset { get; protected set; }

        public bool HasStackUsageInfo { get; set; }
        public int StackIn { get; set; }
        public int StackOut { get; set; }
        public int StackLeftOver { get; set; }

        public void Decode(byte[] code, int offset)
        {
            InitializeDefaultValues(code, offset);
            DecodeInternal(code, offset);
        }

        protected virtual void DecodeInternal(byte[] code, int offset)
        {
            OperandCount = InstructionLength - 1;
            for (int i = 0; i < InstructionLength - 1; i++)
            {
                Operands[i] = code[offset + 1 + i];
            }
        }

        public byte[] Encode()
        {
            return null;
        }

        protected virtual string GetOperandName(int index)
        {
            return null;
        }

        public string GetInstructionText()
        {
            return GetInstructionTextInternal();
        }

        protected virtual string GetInstructionTextInternal()
        {
            var str = new StringBuilder();

            if (!Enum.IsDefined(typeof (OpCode), OpCode))
            {
                str.Append("Unknown_");
                str.Append((int) OpCode);
            }
            else
            {
                str.Append(OpCode);
            }

            for (int i = 0; i < OperandCount; i++)
            {
                str.Append(i > 0 ? ", " : " ");

                object opValue = Operands[i];
                Type opType = opValue.GetType();
                string opName = GetOperandName(i);

                if (opName != null)
                {
                    str.Append(opName);
                    str.Append("=");
                }

                if (opType == typeof (string))
                {
                    str.Append(LiteralFormatter.FormatString(opValue.ToString()));
                }
                else if (opType == typeof(uint))
                {
                    str.Append(LiteralFormatter.FormatInteger((int) (uint) opValue));
                }
                else if (opType == typeof(int))
                {
                    str.Append(LiteralFormatter.FormatInteger((int) opValue));
                }
                else if (opType == typeof(float))
                {
                    str.Append(LiteralFormatter.FormatFloat((float) opValue));
                }
                else
                {
                    str.Append(opValue.ToString());
                }
            }

            return str.ToString();
        }

        public override string ToString()
        {
            return string.Format("{0:x}: ", Offset) + GetInstructionTextInternal();
        }

        protected void InitializeDefaultValues(byte[] code, int offset)
        {
            Offset = offset;

            OpCode = (OpCode) code[offset];
            InstructionLength = GetInstructionLength(code, offset);

            StackUsageInfo info = StackUsage.Get(OpCode);
            if (info != null)
            {
                HasStackUsageInfo = true;
                StackIn = info.In;
                StackOut = info.Out;
                StackLeftOver = info.LeftOver;
            }

            IsConditionalBranch = false;
            BranchOffset = 0;

            OperandCount = 0;
        }

        protected int GetInstructionLength(byte[] code, int offset)
        {
            switch (OpCode)
            {
                case OpCode.StrCpy:
                case OpCode.IntToStr:
                case OpCode.StrCat:
                case OpCode.StrCatI:
                    return 2;
                case OpCode.PushS:
                case OpCode.FnEnd:
                    return 3;
                case OpCode.FnBegin:
                    return 4;
                case OpCode.Jump:
                case OpCode.JumpFalse:
                case OpCode.JumpTrue:
                case OpCode.Push:
                case OpCode.PushF:
                case OpCode.Call:
                    return 5;
                case OpCode.CallNative:
                    return 7;
                case OpCode.Switch:
                    return (code[offset + 1] * 8) + 2;
                case OpCode.PushString:
                    return code[offset + 1] + 2;
                default:
                    return 1;
            }
        }
    }
}