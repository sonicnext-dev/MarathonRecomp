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

namespace RageLib.Scripting.HLScript
{
    internal class StackValueDeref : StackValue
    {
        public StackValueDeref(StackValue pointer)
        {
            ValueType = StackValueType.Unknown;
            ProcessedValue = false;

            if (pointer is StackValuePointerBase)
            {
                Pointer = pointer as StackValuePointerBase;
            }
            else
            {
                UnknownPointer = pointer;
            }
        }

        public StackValuePointerBase Pointer { get; set; }
        public StackValue UnknownPointer { get; set; }

        public override string ToString()
        {
            if (UnknownPointer != null)
            {
                return "*(" + UnknownPointer + ")";
            }
            else
            {
                return Pointer.GetDisplayText();
            }
        }
    }
}