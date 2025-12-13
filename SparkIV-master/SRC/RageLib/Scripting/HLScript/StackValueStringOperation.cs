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
using System.Text;
using RageLib.Scripting.Script;

namespace RageLib.Scripting.HLScript
{
    internal class StackValueStringOperation : StackValueOperation
    {
        private readonly uint _targetBufferSize;
        private readonly uint _sourceBufferSize;

        public StackValueStringOperation(OpCode opCode, uint targetBufferSize)
            : this(opCode, targetBufferSize, 0)
        {
        }

        public StackValueStringOperation(OpCode opCode, uint targetBufferSize, uint sourceBufferSize)
            : base(StackValueOperationType.Unknown)
        {
            Debug.Assert(targetBufferSize % 4 == 0);
            _targetBufferSize = targetBufferSize;

            Debug.Assert(sourceBufferSize % 4 == 0);
            _sourceBufferSize = sourceBufferSize;

            switch (opCode)
            {
                case OpCode.StrCpy:
                    OperationType = StackValueOperationType.StrCpy;
                    ValueType = StackValueType.Unknown;
                    break;
                case OpCode.IntToStr:
                    OperationType = StackValueOperationType.IntToStr;
                    ValueType = StackValueType.Unknown;
                    break;
                case OpCode.StrCat:
                    OperationType = StackValueOperationType.StrCat;
                    ValueType = StackValueType.Unknown;
                    break;
                case OpCode.StrCatI:
                    OperationType = StackValueOperationType.StrCatI;
                    ValueType = StackValueType.Unknown;
                    break;
                case OpCode.StrVarCpy:
                    OperationType = StackValueOperationType.StrVarCpy;
                    ValueType = StackValueType.Unknown;
                    break;
                default:
                    Debug.Assert(false);
                    break;
            }
        }

        public override string ToString()
        {
            var sb = new StringBuilder();

            switch (OperationType)
            {
                case StackValueOperationType.StrCpy:
                    sb.Append("strcpy(");
                    sb.Append(Operands[1]);
                    sb.Append(", ");
                    sb.Append(_targetBufferSize);
                    sb.Append(", ");
                    sb.Append(Operands[0]);
                    sb.Append(")");
                    break;
                case StackValueOperationType.StrCat:
                    sb.Append("strcat(");
                    sb.Append(Operands[1]);
                    sb.Append(", ");
                    sb.Append(_targetBufferSize);
                    sb.Append(", ");
                    sb.Append(Operands[0]);
                    sb.Append(")");
                    break;
                case StackValueOperationType.StrCatI:
                    sb.Append("strcati(");
                    sb.Append(Operands[1]);
                    sb.Append(", ");
                    sb.Append(_targetBufferSize);
                    sb.Append(", ");
                    sb.Append(Operands[0]);
                    sb.Append(")");
                    break;
                case StackValueOperationType.IntToStr:
                    sb.Append("itoa(");
                    sb.Append(Operands[1]);
                    sb.Append(", ");
                    sb.Append(_targetBufferSize);
                    sb.Append(", ");
                    sb.Append(Operands[0]);
                    sb.Append(")");
                    break;
                case StackValueOperationType.StrVarCpy:
                    sb.Append("strvarcpy(");
                    sb.Append(Operands[1]); // target
                    sb.Append(", ");
                    sb.Append(_targetBufferSize); // target size
                    sb.Append(", ");
                    sb.Append(Operands[0]); // source
                    sb.Append(", ");
                    sb.Append(_sourceBufferSize); // source size
                    sb.Append(")");
                    break;
                default:
                    Debug.Assert(false);
                    break;
            }

            return sb.ToString();
        }
    }
}