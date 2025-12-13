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
using System.IO;
using System.Text;
using RageLib.Scripting.Output;
using File=RageLib.Scripting.Script.File;

namespace RageLib.Scripting
{
    public class ScriptFile
    {
        private File _file;

        public void Open(string filename)
        {
            _file = new File();
            if (!_file.Open(filename))
            {
                throw new Exception("Could not load script file.");
            }
        }

        public void Open(Stream stream)
        {
            _file = new File();
            if (!_file.Open(stream))
            {
                throw new Exception("Could not load script file.");
            }
        }

        public string GetCode(CodeFormat format)
        {
            var sb = new StringBuilder();
            var sw = new StringWriter(sb);

            GetCode(format, sw);

            sw.Close();

            return sb.ToString();
        }

        public void GetCode(CodeFormat format, TextWriter writer)
        {
            IOutputProvider output;
            switch (format)
            {
                case CodeFormat.Disassemble:
                    output = OutputFactory.GetDisassembleOutputProvider();
                    break;
                case CodeFormat.ControlFlowDecompile:
                    output = OutputFactory.GetDecompileCFOutputProvider();
                    break;
                case CodeFormat.FullDecompile:
                    output = OutputFactory.GetDecompileFullOutputProvider();
                    break;
                case CodeFormat.FullDecompileAnnotate:
                    output = OutputFactory.GetDecompileFullAnnotateOutputProvider();
                    break;
                case CodeFormat.CodePath:
                    output = OutputFactory.GetCodePathOutputProvider();
                    break;
                case CodeFormat.Variables:
                    output = OutputFactory.GetVariablesOutputProvider();
                    break;
                case CodeFormat.ScruffDecompile:
                    output = OutputFactory.GetScruffDecompileOutputProvider();
                    break;
                case CodeFormat.ScruffHeader:
                    output = OutputFactory.GetScruffHeaderOutputProvider();
                    break;
                default:
                    throw new ArgumentOutOfRangeException("format");
            }

            output.Process(_file, writer);
        }

        public uint[] LocalVars
        {
            get
            {
                return _file.LocalVars;
            }
        }

        public uint[] GlobalVars
        {
            get
            {
                return _file.GlobalVars;
            }
        }
    }
}