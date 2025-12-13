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

namespace RageLib.Scripting.HLScript
{
    internal class StackValuePointerVar : StackValuePointerBase
    {
        public StackValuePointerType PointerType;

        public StackValuePointerVar(StackValuePointerType type)
        {
            ValueType = StackValueType.Pointer;
            PointerType = type;
        }

        public StackValuePointerVar(StackValuePointerType type, int pointerIndex)
            : this(type)
        {
            PointerIndex = pointerIndex;
        }

        public int PointerIndex { get; set; }

        public override string GetDisplayText()
        {
            var sb = new StringBuilder();
            switch (PointerType)
            {
                case StackValuePointerType.Local:
                    sb.Append("L");
                    break;
                case StackValuePointerType.Global:
                    sb.Append("G");
                    break;
                case StackValuePointerType.Null:
                    sb.Append("null");
                    break;
                case StackValuePointerType.Stack:
                    sb.Append("var");
                    break;
                case StackValuePointerType.Temporary:
                    sb.Append("temp");
                    break;
                default:
                    Debug.Assert(false);
                    break;
            }

            if (PointerType == StackValuePointerType.Stack || PointerType == StackValuePointerType.Temporary )
            {
                sb.Append(PointerIndex);
            }
            else if (PointerType != StackValuePointerType.Null)
            {
                sb.Append("[");
                sb.Append(PointerIndex);
                sb.Append("]");
            }

            return sb.ToString();
        }

        public override string ToString()
        {
            return "&" + GetDisplayText();
        }

    }
}