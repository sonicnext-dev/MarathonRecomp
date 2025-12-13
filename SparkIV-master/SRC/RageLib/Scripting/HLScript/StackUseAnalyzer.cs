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
using System.Diagnostics;
using RageLib.Scripting.Script;

namespace RageLib.Scripting.HLScript
{
    internal class StackUseAnalyzer
    {
        public StackUseAnalyzer(ScriptProgram program)
        {
            Program = program;
        }

        private ScriptProgram Program { get; set; }

        private StackValuePointerBase PopPointer(Stack<StackValue> stack)
        {
            StackValue value = stack.Pop();
            if (value is StackValuePointerBase)
            {
                return value as StackValuePointerBase;
            }
            else
            {
                return new StackValuePointerFake(value);
            }
        }

        private StackValuePointerBase PeekPointer(Stack<StackValue> stack)
        {
            StackValue value = stack.Peek();
            if (value is StackValuePointerBase)
            {
                return value as StackValuePointerBase;
            }
            else
            {
                return new StackValuePointerFake(value);
            }
        }

        private void VectorOperation(StackValueOperationType type, Stack<StackValue> stack)
        {
            var vec1 = new StackValue[3];
            var vec2 = new StackValue[3];

            if (type != StackValueOperationType.FromF)
            {
                vec1[0] = stack.Pop();
                vec1[1] = stack.Pop();
                vec1[2] = stack.Pop();

                if (type != StackValueOperationType.Neg)
                {
                    vec2[0] = stack.Pop();
                    vec2[1] = stack.Pop();
                    vec2[2] = stack.Pop();

                    stack.Push(new StackValueOperation(type, StackValueType.Float, new[] {vec1[2], vec2[2]}));
                    stack.Push(new StackValueOperation(type, StackValueType.Float, new[] {vec1[1], vec2[1]}));
                    stack.Push(new StackValueOperation(type, StackValueType.Float, new[] {vec1[0], vec2[0]}));
                }
                else
                {
                    stack.Push(new StackValueOperation(type, StackValueType.Float, new[] {vec1[2]}));
                    stack.Push(new StackValueOperation(type, StackValueType.Float, new[] {vec1[1]}));
                    stack.Push(new StackValueOperation(type, StackValueType.Float, new[] {vec1[0]}));
                }
            }
            else
            {
                vec1[0] = stack.Pop();

                stack.Push(vec1[0]);
                stack.Push(vec1[0]);
                stack.Push(vec1[0]);
            }
        }

        private void AnalyzeCodePath(Stack<StackValue> stack, CodePath path, ref int tempVarIndex)
        {
            HLInstruction instruction = path.GetFirstInstruction();
            while (instruction != null)
            {
                try
                {
                    if (instruction.BranchFunction != null)
                    {
                        if (!instruction.Instruction.HasStackUsageInfo)
                        {
                            instruction.Instruction.StackIn = instruction.BranchFunction.ParameterCount;
                            instruction.Instruction.StackOut = instruction.BranchFunction.ReturnCount;
                            instruction.Instruction.StackLeftOver = 0;
                            instruction.Instruction.HasStackUsageInfo = true;
                        }
                    }

                    if (instruction.UnconditionalBranch)
                    {
                        // One should not even occur
                        //Debug.Assert(false);
                    }
                    else if (instruction.ExitFunction)
                    {
                        var value = new StackValueOperation(StackValueOperationType.Return);

                        if (path.ParentFunction.ReturnCount > 0)
                        {
                            for (int i = 0; i < path.ParentFunction.ReturnCount; i++)
                            {
                                value.Operands.Add(stack.Pop());
                            }
                        }

                        instruction.ProcessedStackValue = value;
                    }
                    else if (instruction.IsConditionalBranch || instruction.IsSwitchBranch)
                    {
                        instruction.ProcessedStackValue = stack.Pop();
                    }
                    else
                    {
                        bool processDefault = false;

                        StackValueOperation operationValue = null;
                        StackValue tempAnyValue;
                        StackValueOperation tempOpValue;
                        StackValueLiteral tempLiteralValue;
                        StackValuePointerBase tempPointerValue;

                        OpCode opCode = instruction.Instruction.OpCode;
                        switch (opCode)
                        {
                            case OpCode.AddVec:
                                VectorOperation(StackValueOperationType.Add, stack);
                                break;
                            case OpCode.SubVec:
                                VectorOperation(StackValueOperationType.Sub, stack);
                                break;
                            case OpCode.MulVec:
                                VectorOperation(StackValueOperationType.Mul, stack);
                                break;
                            case OpCode.DivVec:
                                VectorOperation(StackValueOperationType.Div, stack);
                                break;
                            case OpCode.NegVec:
                                VectorOperation(StackValueOperationType.Neg, stack);
                                break;
                            case OpCode.VecFromF:
                                VectorOperation(StackValueOperationType.FromF, stack);
                                break;
                            case OpCode.PushS:
                                stack.Push(new StackValueLiteral((short) instruction.Instruction.Operands[0]));
                                break;
                            case OpCode.Push:
                                stack.Push(new StackValueLiteral((int) (uint) instruction.Instruction.Operands[0]));
                                break;
                            case OpCode.PushF:
                                stack.Push(new StackValueLiteral((float) instruction.Instruction.Operands[0]));
                                break;
                            case OpCode.PushString:
                                stack.Push(new StackValueLiteral((string) instruction.Instruction.Operands[0]));
                                break;
                            case OpCode.Dup:
                                stack.Push(stack.Peek());
                                break;
                            case OpCode.Pop:
                                tempOpValue = stack.Peek() as StackValueOperation;
                                if (tempOpValue != null && tempOpValue.OperationType == StackValueOperationType.Call)
                                {
                                    operationValue = new StackValueOperation(StackValueOperationType.Pop);
                                    processDefault = true;
                                }
                                break;
                            case OpCode.CallNative:
                                operationValue = new StackValueOperation(instruction.Instruction.Operands[2].ToString());
                                processDefault = true;
                                break;
                            case OpCode.Call:
                                operationValue = new StackValueOperation(instruction.BranchFunction.Name);
                                processDefault = true;
                                break;
                            case OpCode.RefGet:
                                stack.Push(new StackValueDeref(stack.Pop()));
                                break;
                            case OpCode.RefSet:
                                instruction.ProcessedStackValue = new StackValueAssign(PopPointer(stack), stack.Pop());
                                break;
                            case OpCode.RefPeekSet:
                                tempAnyValue = stack.Pop(); // value!
                                instruction.ProcessedStackValue = new StackValueAssign(PeekPointer(stack), tempAnyValue);
                                break;
                            case OpCode.ArrayExplode:
                                // This is used to either pass a structure to a native/function call
                                // or to explode the variables onto the stack to do a "shallow-copy"

                                tempPointerValue = PopPointer(stack);
                                tempLiteralValue = stack.Pop() as StackValueLiteral;
                                Debug.Assert(tempLiteralValue != null &&
                                             tempLiteralValue.ValueType == StackValueType.Integer);

                                var explodeCount = (int) tempLiteralValue.Value;
                                for (int i = 0; i < explodeCount; i++)
                                {
                                    stack.Push(new StackValueDeref(new StackValuePointerIndex(tempPointerValue, i)));
                                }

                                break;
                            case OpCode.ArrayImplode:
                                // The only case this is ever used is for a shallow copy!

                                tempPointerValue = PopPointer(stack);
                                tempLiteralValue = stack.Pop() as StackValueLiteral;
                                Debug.Assert(tempLiteralValue != null &&
                                             tempLiteralValue.ValueType == StackValueType.Integer);

                                var tempStack = new Stack<StackValue>();
                                var implodeCount = (int) tempLiteralValue.Value;
                                for (int i = implodeCount - 1; i >= 0; i--)
                                {
                                    tempStack.Push(
                                        new StackValueAssign(new StackValuePointerIndex(tempPointerValue, i),
                                                             stack.Pop()));
                                }

                                var stackValueGroup = new ProcessedStackValueGroup();

                                stackValueGroup.Values.AddRange(tempStack.ToArray());
                                instruction.ProcessedStackValue = stackValueGroup;

                                break;
                            case OpCode.Var0:
                            case OpCode.Var1:
                            case OpCode.Var2:
                            case OpCode.Var3:
                            case OpCode.Var4:
                            case OpCode.Var5:
                            case OpCode.Var6:
                            case OpCode.Var7:
                                stack.Push(new StackValuePointerVar(StackValuePointerType.Stack,
                                                                    (opCode - OpCode.Var0)));
                                break;
                            case OpCode.Var:
                                tempLiteralValue = stack.Pop() as StackValueLiteral;
                                Debug.Assert(tempLiteralValue != null &&
                                             tempLiteralValue.ValueType == StackValueType.Integer);

                                stack.Push(new StackValuePointerVar(StackValuePointerType.Stack,
                                                                    (int) tempLiteralValue.Value));
                                break;
                            case OpCode.LocalVar:
                                tempLiteralValue = stack.Pop() as StackValueLiteral;
                                Debug.Assert(tempLiteralValue != null &&
                                             tempLiteralValue.ValueType == StackValueType.Integer);

                                stack.Push(new StackValuePointerVar(StackValuePointerType.Local,
                                                                    (int) tempLiteralValue.Value));
                                break;
                            case OpCode.GlobalVar:
                                tempLiteralValue = stack.Pop() as StackValueLiteral;
                                Debug.Assert(tempLiteralValue != null &&
                                             tempLiteralValue.ValueType == StackValueType.Integer);

                                stack.Push(new StackValuePointerVar(StackValuePointerType.Global,
                                                                    (int) tempLiteralValue.Value));
                                break;
                            case OpCode.ArrayRef:
                                tempPointerValue = PopPointer(stack);
                                tempLiteralValue = stack.Pop() as StackValueLiteral;
                                tempAnyValue = stack.Pop();
                                Debug.Assert(tempPointerValue != null);
                                Debug.Assert(tempLiteralValue != null &&
                                             tempLiteralValue.ValueType == StackValueType.Integer);

                                stack.Push(new StackValuePointerArray(tempPointerValue, tempAnyValue,
                                                                      (int) tempLiteralValue.Value));
                                break;
                            case OpCode.NullObj:
                                stack.Push(new StackValuePointerVar(StackValuePointerType.Null));
                                break;
                            case OpCode.Add:
                                // Special case for pointer add
                                tempAnyValue = stack.Pop();
                                tempPointerValue = stack.Peek() as StackValuePointerBase;

                                if (tempPointerValue != null)
                                {
                                    stack.Pop(); // tempPointerValue

                                    tempLiteralValue = tempAnyValue as StackValueLiteral;
                                    Debug.Assert(tempLiteralValue != null && (int) tempLiteralValue.Value%4 == 0);

                                    stack.Push(new StackValuePointerIndex(tempPointerValue,
                                                                          (int) tempLiteralValue.Value/4));
                                }
                                else
                                {
                                    stack.Push(tempAnyValue);
                                    operationValue = new StackValueOperation(opCode);
                                    processDefault = true;
                                }
                                break;
                            case OpCode.StrCat:
                            case OpCode.StrCatI:
                            case OpCode.StrCpy:
                            case OpCode.IntToStr:
                                operationValue = new StackValueStringOperation(opCode, (byte)instruction.Instruction.Operands[0]);
                                processDefault = true;
                                break;
                            case OpCode.StrVarCpy:
                                tempPointerValue = PopPointer(stack);

                                tempLiteralValue = stack.Pop() as StackValueLiteral;
                                
                                Debug.Assert(tempLiteralValue != null &&
                                             tempLiteralValue.ValueType == StackValueType.Integer);

                                var targetSize = (uint)(int)tempLiteralValue.Value;

                                tempLiteralValue = stack.Pop() as StackValueLiteral;

                                Debug.Assert(tempLiteralValue != null &&
                                             tempLiteralValue.ValueType == StackValueType.Integer);

                                var sourceSize = (uint)(int)tempLiteralValue.Value;

                                var popSize = sourceSize;

                                tempAnyValue = PopPointer(stack);

                                // Pop till the last one
                                for (var i = 1; i < popSize; i++)
                                {
                                    tempAnyValue = PopPointer(stack);
                                }

                                operationValue = new StackValueStringOperation(opCode, targetSize, sourceSize);
                                operationValue.Operands.Add(new StackValueRef(tempAnyValue)); // 0 = source
                                operationValue.Operands.Add(tempPointerValue); // 1 = target
                                
                                instruction.ProcessedStackValue = operationValue;

                                break;
                            case OpCode.RefProtect:
                                tempLiteralValue = stack.Pop() as StackValueLiteral;

                                Debug.Assert(tempLiteralValue != null &&
                                     tempLiteralValue.ValueType == StackValueType.Integer);

                                var protectMode = (int)tempLiteralValue.Value;

                                Debug.Assert(protectMode == 1 || protectMode == 2 || protectMode == 5 || protectMode == 6);

                                tempLiteralValue = stack.Pop() as StackValueLiteral;

                                Debug.Assert(tempLiteralValue != null &&
                                     tempLiteralValue.ValueType == StackValueType.Integer);

                                var protectCount = (int)tempLiteralValue.Value;

                                //Debug.Assert(protectCount == 1);

                                tempPointerValue = PopPointer(stack);

                                operationValue = new StackValueOperation(StackValueOperationType.RefProtect);
                                operationValue.Operands.Add(tempPointerValue);
                                operationValue.Operands.Add(tempLiteralValue);

                                if ((protectMode & 1) != 0)
                                {
                                    stack.Push(operationValue);
                                }
                                
                                break;
                            default:
                                if (instruction.Instruction is InstructionPush)
                                {
                                    // PushD special case
                                    stack.Push(new StackValueLiteral((int) instruction.Instruction.Operands[0]));
                                }
                                else
                                {
                                    operationValue = new StackValueOperation(opCode);
                                    processDefault = true;
                                }
                                break;
                        }

                        if (processDefault)
                        {
                            Debug.Assert(instruction.Instruction.HasStackUsageInfo);

                            var stackValues = new Stack<StackValue>();
                            for (int i = 0; i < instruction.Instruction.StackIn; i++)
                            {
                                StackValue stackItem = stack.Pop();
                                stackValues.Push(stackItem);
                            }

                            operationValue.Operands.AddRange(stackValues);

                            if (instruction.Instruction.StackLeftOver > 0)
                            {
                                for (int i = 0; i < instruction.Instruction.StackLeftOver; i++)
                                {
                                    stackValues.Push(stackValues.Pop());
                                }
                            }

                            if (instruction.Instruction.StackOut > 0)
                            {
                                if (instruction.Instruction.StackOut > 1)
                                {
                                    var multiAssign = new StackValueAssignMulti(operationValue);
                                    for (int i = 0; i < instruction.Instruction.StackOut; i++)
                                    {
                                        tempPointerValue = new StackValuePointerVar(StackValuePointerType.Temporary,
                                                                                    tempVarIndex + i);
                                        multiAssign.AssignPointers.Add(tempPointerValue);
                                        stack.Push(new StackValueDeref(tempPointerValue));
                                    }
                                    tempVarIndex += instruction.Instruction.StackOut;
                                    instruction.ProcessedStackValue = multiAssign;
                                }
                                else
                                {
                                    stack.Push(operationValue);
                                }
                            }
                            else
                            {
                                instruction.ProcessedStackValue = operationValue;
                            }
                        }
                    }
                }
                catch (Exception e)
                {
                    throw new Exception(
                        "Error while decompiling instruction : " + instruction.Instruction + "\n", e);
                }

                instruction = instruction.GetNextInstruction();
            }
        }


        private void AnalyzeFunction(Function function)
        {
            int tempVarIndex = 1;

            foreach (CodePath path in function.CodePaths)
            {
                var stack = new Stack<StackValue>();
                stack.Push(new StackValueLiteral(0));
                AnalyzeCodePath(stack, path, ref tempVarIndex);
            }

            function.TemporaryCount = tempVarIndex - 1;
        }

        public void Analyze()
        {
            foreach (Function fn in Program.Functions)
            {
                AnalyzeFunction(fn);
            }
        }
    }
}