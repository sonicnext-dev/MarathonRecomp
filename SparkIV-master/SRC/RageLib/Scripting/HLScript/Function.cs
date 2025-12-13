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
using RageLib.Scripting.Script;

namespace RageLib.Scripting.HLScript
{
    internal class Function
    {
        public Function()
        {
            CodePaths = new List<CodePath>();

            CreateCodePath("main");
        }

        public Instruction Instruction { get; set; }

        public int StartOffset { get; set; }
        public int EndOffset { get; set; }
        public string Name { get; set; }

        public CodePath MainCodePath
        {
            get { return CodePaths[0]; }
        }

        public List<CodePath> CodePaths { get; private set; }

        public int ParameterCount { get; set; }
        public int VariableCount { get; set; }
        public int TemporaryCount { get; set; }
        public int ReturnCount { get; set; }

        public CodePath CreateCodePath(string name)
        {
            var path = new CodePath();
            path.Name = name;
            path.ParentFunction = this;
            CodePaths.Add(path);
            return path;
        }

        public CodePath DetermineCodePathFromOffset(int offset)
        {
            foreach (CodePath path in CodePaths)
            {
                if (path.GetInstruction(offset) != null)
                {
                    return path;
                }
            }
            return null;
        }

        public override string ToString()
        {
            return Name;
        }
    }
}