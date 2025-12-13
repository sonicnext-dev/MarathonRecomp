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

namespace RageLib.Scripting.Script
{
    internal static class StackUsage
    {
        private static readonly Dictionary<OpCode, StackUsageInfo> _entries = new Dictionary<OpCode, StackUsageInfo>();

        static StackUsage()
        {
            AddEntry(OpCode.Add, 2, 1);
            AddEntry(OpCode.Sub, 2, 1);
            AddEntry(OpCode.Mul, 2, 1);
            AddEntry(OpCode.Div, 2, 1);
            AddEntry(OpCode.Mod, 2, 1);
            AddEntry(OpCode.IsZero, 1, 1);
            AddEntry(OpCode.Neg, 1, 1);
            AddEntry(OpCode.CmpEq, 2, 1);
            AddEntry(OpCode.CmpNe, 2, 1);
            AddEntry(OpCode.CmpGt, 2, 1);
            AddEntry(OpCode.CmpGe, 2, 1);
            AddEntry(OpCode.CmpLt, 2, 1);
            AddEntry(OpCode.CmpLe, 2, 1);
            AddEntry(OpCode.AddF, 2, 1);
            AddEntry(OpCode.SubF, 2, 1);
            AddEntry(OpCode.MulF, 2, 1);
            AddEntry(OpCode.DivF, 2, 1);
            AddEntry(OpCode.ModF, 2, 1);
            AddEntry(OpCode.NegF, 1, 1);
            AddEntry(OpCode.CmpEqF, 2, 1);
            AddEntry(OpCode.CmpNeF, 2, 1);
            AddEntry(OpCode.CmpGtF, 2, 1);
            AddEntry(OpCode.CmpGeF, 2, 1);
            AddEntry(OpCode.CmpLtF, 2, 1);
            AddEntry(OpCode.CmpLeF, 2, 1);
            AddEntry(OpCode.AddVec, 6, 3);
            AddEntry(OpCode.SubVec, 6, 3);
            AddEntry(OpCode.MulVec, 6, 3);
            AddEntry(OpCode.DivVec, 6, 3);
            AddEntry(OpCode.NegVec, 3, 3);
            AddEntry(OpCode.And, 2, 1);
            AddEntry(OpCode.Or, 2, 1);
            AddEntry(OpCode.Xor, 2, 1);
            AddEntry(OpCode.Jump, 0, 0);
            AddEntry(OpCode.JumpFalse, 1, 0);
            AddEntry(OpCode.JumpTrue, 1, 0);
            AddEntry(OpCode.ToF, 1, 1);
            AddEntry(OpCode.FromF, 1, 1);
            AddEntry(OpCode.VecFromF, 1, 3);
            AddEntry(OpCode.PushS, 0, 1);
            AddEntry(OpCode.Push, 0, 1);
            AddEntry(OpCode.PushF, 0, 1);
            AddEntry(OpCode.Dup, 1, 1, 1);
            AddEntry(OpCode.Pop, 1, 0);

            //AddEntry(OpCode.CallNative, 0, 0);      SPECIAL CASE, We'll deal with it later
            //AddEntry(OpCode.Call, 0, 0);            Another special case... the function call needs to be parsed before we can determine this!
            //AddEntry(OpCode.FnBegin, 0, 0);
            //AddEntry(OpCode.FnEnd, 0, 0);

            AddEntry(OpCode.RefGet, 1, 1);
            AddEntry(OpCode.RefSet, 2, 0);
            AddEntry(OpCode.RefPeekSet, 2, 0, 1);

            /*

            ArrayExplode,     // Expands N items off an array onto the stack
                              // Pops a ptr off the stack (A), pops another ptr off the stack (B), deferences B to get (N).

            ArrayImplode,     // Takes N items off the stack, and stores them in the array (similar to explode)
                              // Note that the topmost item on the stack will be the last item on the stack (so that we can explode again)

             */

            AddEntry(OpCode.Var0, 0, 1);
            AddEntry(OpCode.Var1, 0, 1);
            AddEntry(OpCode.Var2, 0, 1);
            AddEntry(OpCode.Var3, 0, 1);
            AddEntry(OpCode.Var4, 0, 1);
            AddEntry(OpCode.Var5, 0, 1);
            AddEntry(OpCode.Var6, 0, 1);
            AddEntry(OpCode.Var7, 0, 1);

            AddEntry(OpCode.Var, 1, 1);

            AddEntry(OpCode.LocalVar, 1, 1);
            AddEntry(OpCode.GlobalVar, 1, 1);
            AddEntry(OpCode.ArrayRef, 3, 1);

            AddEntry(OpCode.Switch, 1, 0);
            AddEntry(OpCode.PushString, 0, 1);
            AddEntry(OpCode.NullObj, 0, 1);

            AddEntry(OpCode.StrCpy, 2, 0);
            AddEntry(OpCode.IntToStr, 2, 0);
            AddEntry(OpCode.StrCat, 2, 0);
            AddEntry(OpCode.StrCatI, 2, 0);

            AddEntry(OpCode.Catch, 0, 1);
            //AddEntry(OpCode.Throw, ?, ?);     // ARBITRARY
            //AddEntry(OpCode.StrVarCpy, ?, ?);     // ARBITRARY

            AddEntry(OpCode.SetProtect, 2, 0);
            AddEntry(OpCode.GetProtect, 1, 1);
            // AddEntry(OpCode.RefProtect, ?, ?);

            for (int i = 80; i < 256; i++)
            {
                AddEntry((OpCode) i, 0, 1);
            }
        }

        private static void AddEntry(OpCode opcode, int stackIn, int stackOut)
        {
            _entries.Add(opcode, new StackUsageInfo(stackIn, stackOut, 0));
        }

        private static void AddEntry(OpCode opcode, int stackIn, int stackOut, int stackLeftOver)
        {
            _entries.Add(opcode, new StackUsageInfo(stackIn, stackOut, stackLeftOver));
        }

        public static StackUsageInfo Get(OpCode opcode)
        {
            if (_entries.ContainsKey(opcode))
            {
                return _entries[opcode];
            }
            else
            {
                return null;
            }
        }
    }
}