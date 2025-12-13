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
using System.Diagnostics;
using System.Text;
using RageLib.Scripting.Script;

namespace RageLib.Scripting.HLScript
{
    internal class StackValueOperation : StackValue
    {
        public StackValueOperation(StackValueOperationType type, StackValueType valueType,
                                   IEnumerable<StackValue> operands)
        {
            ProcessedValue = true;
            ValueType = valueType;
            if (operands == null)
            {
                Operands = new List<StackValue>();
            }
            else
            {
                Operands = new List<StackValue>(operands);
            }
            OperationType = type;
        }

        public StackValueOperation(StackValueOperationType type)
            : this(type, StackValueType.Unknown, null)
        {
        }

        public StackValueOperation(OpCode opCode)
            : this(StackValueOperationType.Unknown)
        {
            switch (opCode)
            {
                case OpCode.Add:
                    OperationType = StackValueOperationType.Add;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.Sub:
                    OperationType = StackValueOperationType.Sub;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.Mul:
                    OperationType = StackValueOperationType.Mul;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.Div:
                    OperationType = StackValueOperationType.Div;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.Mod:
                    OperationType = StackValueOperationType.Mod;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.IsZero:
                    OperationType = StackValueOperationType.IsZero;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.Neg:
                    OperationType = StackValueOperationType.Neg;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.CmpEq:
                    OperationType = StackValueOperationType.CmpEq;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.CmpNe:
                    OperationType = StackValueOperationType.CmpNe;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.CmpGt:
                    OperationType = StackValueOperationType.CmpGt;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.CmpGe:
                    OperationType = StackValueOperationType.CmpGe;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.CmpLt:
                    OperationType = StackValueOperationType.CmpLt;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.CmpLe:
                    OperationType = StackValueOperationType.CmpLe;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.AddF:
                    OperationType = StackValueOperationType.Add;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.SubF:
                    OperationType = StackValueOperationType.Sub;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.MulF:
                    OperationType = StackValueOperationType.Mul;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.DivF:
                    OperationType = StackValueOperationType.Div;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.ModF:
                    OperationType = StackValueOperationType.Mod;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.NegF:
                    OperationType = StackValueOperationType.Neg;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.CmpEqF:
                    OperationType = StackValueOperationType.CmpEq;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.CmpNeF:
                    OperationType = StackValueOperationType.CmpNe;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.CmpGtF:
                    OperationType = StackValueOperationType.CmpGt;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.CmpGeF:
                    OperationType = StackValueOperationType.CmpGe;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.CmpLtF:
                    OperationType = StackValueOperationType.CmpLt;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.CmpLeF:
                    OperationType = StackValueOperationType.CmpLe;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.And:
                    OperationType = StackValueOperationType.And;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.Or:
                    OperationType = StackValueOperationType.Or;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.Xor:
                    OperationType = StackValueOperationType.Xor;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.ToF:
                    OperationType = StackValueOperationType.ToF;
                    ValueType = StackValueType.Float;
                    break;
                case OpCode.FromF:
                    OperationType = StackValueOperationType.FromF;
                    ValueType = StackValueType.Integer;
                    break;
                case OpCode.Pop:
                    OperationType = StackValueOperationType.Pop;
                    ValueType = StackValueType.Unknown;
                    break;
                case OpCode.GetProtect:
                    OperationType = StackValueOperationType.GetProtect;
                    ValueType = StackValueType.Unknown;
                    break;
                case OpCode.SetProtect:
                    OperationType = StackValueOperationType.SetProtect;
                    ValueType = StackValueType.Unknown;
                    break;
                default:
                    Debug.Assert(false);
                    break;
            }
        }

        public StackValueOperation(string callFunctioName)
            : this(StackValueOperationType.Call)
        {
            CallFunctionName = callFunctioName;
            ProcessedValue = false;
        }

        public StackValueOperationType OperationType { get; set; }
        public List<StackValue> Operands { get; private set; }

        public string CallFunctionName { get; set; }

        private string GetFormattedOperand(int index)
        {
            StackValue sv = Operands[index];
            if (sv.ProcessedValue)
            {
                return "(" + sv + ")";
            }
            else
            {
                return sv.ToString();
            }
        }

        public override string ToString()
        {
            var sb = new StringBuilder();

            switch (OperationType)
            {
                case StackValueOperationType.Add:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" + ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.Sub:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" - ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.Mul:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" * ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.Div:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" / ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.Mod:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" % ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.Neg:
                    sb.Append("-");
                    sb.Append(GetFormattedOperand(0));
                    break;
                case StackValueOperationType.And:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" && ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.Or:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" || ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.Xor:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" ^ ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.CmpEq:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" == ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.CmpNe:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" != ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.CmpGt:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" > ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.CmpGe:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" >= ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.CmpLt:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" < ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.CmpLe:
                    sb.Append(GetFormattedOperand(0));
                    sb.Append(" <= ");
                    sb.Append(GetFormattedOperand(1));
                    break;
                case StackValueOperationType.IsZero:
                    sb.Append("!");
                    sb.Append(GetFormattedOperand(0));
                    break;
                case StackValueOperationType.ToF:
                    sb.Append("(float)");
                    sb.Append(GetFormattedOperand(0));
                    break;
                case StackValueOperationType.FromF:
                    sb.Append("(int)");
                    sb.Append(GetFormattedOperand(0));
                    break;
                case StackValueOperationType.Return:
                    sb.Append("return");
                    if (Operands.Count > 0)
                    {
                        sb.Append(" ");
                        for (int i = Operands.Count - 1; i >= 0; i--)
                        {
                            sb.Append(Operands[i]);
                            if (i > 0)
                            {
                                sb.Append(", ");
                            }
                        }
                    }
                    break;
                case StackValueOperationType.Call:
                    sb.Append(CallFunctionName);

                    sb.Append("(");

                    for (int i = 0; i < Operands.Count; i++)
                    {
                        if (i > 0)
                        {
                            sb.Append(", ");
                        }
                        sb.Append(Operands[i].ToString());
                    }

                    sb.Append(")");

                    break;
                case StackValueOperationType.Pop:
                    sb.Append(Operands[0].ToString());
                    break;
                case StackValueOperationType.SetProtect:
                    sb.Append("__pset(");
                    sb.Append(Operands[1].ToString());
                    sb.Append(", ");
                    sb.Append(Operands[0].ToString());
                    sb.Append(")");
                    break;
                case StackValueOperationType.GetProtect:
                    sb.Append("__pget(");
                    sb.Append(Operands[0].ToString());
                    sb.Append(")");
                    break;
                case StackValueOperationType.RefProtect:
                    sb.Append("__unprot(");
                    sb.Append(Operands[0].ToString());
                    sb.Append(", ");
                    sb.Append(Operands[1].ToString());
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