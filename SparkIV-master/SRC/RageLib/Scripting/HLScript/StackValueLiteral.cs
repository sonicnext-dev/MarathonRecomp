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
using RageLib.Scripting.Script;

namespace RageLib.Scripting.HLScript
{
    internal class StackValueLiteral : StackValue
    {
        public StackValueLiteral(string value)
        {
            ValueType = StackValueType.String;
            Value = value;
        }

        public StackValueLiteral(int value)
        {
            ValueType = StackValueType.Integer;
            Value = value;
        }

        public StackValueLiteral(float value)
        {
            ValueType = StackValueType.Float;
            Value = value;
        }

        public object Value { get; set; }

        public T GetValue<T>()
        {
            return (T) Value;
        }

        public override string ToString()
        {
            switch (ValueType)
            {
                case StackValueType.Integer:
                    return LiteralFormatter.FormatInteger((int) Value);
                case StackValueType.Float:
                    return LiteralFormatter.FormatFloat((float) Value);
                case StackValueType.String:
                    return LiteralFormatter.FormatString(Value.ToString());
                default:
                    Debug.Assert(false);
                    return null;
            }
        }
    }
}