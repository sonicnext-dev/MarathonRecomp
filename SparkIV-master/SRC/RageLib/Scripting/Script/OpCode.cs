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

using System.Reflection;

namespace RageLib.Scripting.Script
{
    [Obfuscation(StripAfterObfuscation = true, ApplyToMembers = true, Exclude = true)]
    internal enum OpCode
    {
        Add = 1,
        Sub, // Subtractee on top of stack (THINK RPN)
        Mul,
        Div, // Divisor on top of stack

        Mod,

        IsZero,

        Neg,

        CmpEq,
        CmpNe,
        CmpGt,
        CmpGe,
        CmpLt,
        CmpLe,

        AddF,
        SubF,
        MulF,
        DivF,

        ModF, // *I THINK*

        NegF,

        CmpEqF,
        CmpNeF,
        CmpGtF,
        CmpGeF,
        CmpLtF,
        CmpLeF,

        AddVec, // 26     Add 2 Vec3s
        SubVec, // 27
        MulVec, // multiply each element together returning a Vec3
        DivVec,

        NegVec,

        And,
        Or,
        Xor,

        Jump, // 34 Unconditonal Jump (IP addr in next 4 bytes)
        JumpFalse, // Jump if stack has 0
        JumpTrue,

        ToF,
        FromF, // 38

        VecFromF, // Converts to a 3 float vector from 1 float on stack

        PushS, // Loads next 2 bytes sign extended

        Push, // Loads next 4 bytes (as unsigned int)
        PushF, // Loads next 4 bytes (as float) -- fyi, this is executed by the same code that does Push

        Dup, // Duplicates whatever is on the stack

        Pop, // Drops the last value on the stack

        CallNative, // 45.. Calls a native command

        Call, // Pushes IP onto stack and jump to IP in next 4 bytes

        FnBegin, // params: input stack #, variables on stack # (int16)
        FnEnd, // params: input popped #, return stack #

        RefGet, // Pops a ptr off the stack (A), deferences A (B), and pushes B onto the stack
        // This could also be ArrayCount... because the number of array elements is stored at index [0]

        RefSet,
        // Pops a value off the stack (A), pops one more off the stack (B), stores B at *A        (single ref op)
        RefPeekSet,
        // Pops a value off the stack (A), peeks one more off the stack (B), stores A at *B       (probably for multiple ref op)

        ArrayExplode, // Expands N items off an array onto the stack
        // Pops a ptr off the stack (A), pops another ptr off the stack (B), deferences B to get (N).

        ArrayImplode, // Takes N items off the stack, and stores them in the array (similar to explode)
        // Note that the topmost item on the stack will be the last item on the stack (so that we can explode again)

        Var0, // Same code is executed for 55 56 57 58 59 60 61
        Var1, // Gets ptr to variable onto stack
        Var2, // Note that params are on top of stack, followed by locals....
        Var3, // When returning, push the return variables onto top of stack
        Var4, // Should this be 0 based instead of 1 based?? -- YES
        Var5,
        Var6,
        Var7,

        Var, // Same as the ones above but for index >= 8

        LocalVar, // Gets a script local variable (not function local!)

        GlobalVar, // Gets a ptr to the global var at the index specified on the stack

        ArrayRef,
        // Gets a ptr to an array index... specify array, element size, and index on stack. Note that actual byte size of each element is 4.

        Switch,
        // Stack contains index to jump to... operands: byte: number of entries (A), [ int: index, int: offset ] repeated A times

        PushString,
        // Gets 1 byte from code to determine string length (N), pushes the string address onto the stack, and then IP += N + 1;

        NullObj, // Pushes an unknown table ptr onto the stack (%r23 ==> .data:830F4FB4 unk_830F4FB4)
                    // All code seems to deref this ptr directly and not index into it
                    // Its never written to by the scripts, only read from.
                    // Best guess is that its a null/disposed object reference

        StrCpy,

        IntToStr,

        StrCat, // creds to listener

        StrCatI, // cats an integer, creds to listener

        Catch, // Related to link stack + local stack start        (creds to listener) -- NOT USED??

        Throw, // (creds to listener) -- NOT USED??

        StrVarCpy, // pops A, B, C from stack. copies C * 4 bytes into *A which can hold B * 4 + 1 bytes, appends a 0 to A.

        GetProtect,

        SetProtect,

        RefProtect,

        Abort_79,
    }
}