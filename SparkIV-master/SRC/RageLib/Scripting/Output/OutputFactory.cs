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

namespace RageLib.Scripting.Output
{
    internal class OutputFactory
    {
        public static IOutputProvider GetDecompileCFOutputProvider()
        {
            return new DecompileCFOutput();
        }

        public static IOutputProvider GetDecompileFullAnnotateOutputProvider()
        {
            return new DecompileFullOutput(true);
        }

        public static IOutputProvider GetDecompileFullOutputProvider()
        {
            return new DecompileFullOutput(false);
        }

        public static IOutputProvider GetCodePathOutputProvider()
        {
            return new CodePathOutput();
        }

        public static IOutputProvider GetDisassembleOutputProvider()
        {
            return new DisassembleOutput();
        }

        public static IOutputProvider GetVariablesOutputProvider()
        {
            return new VariablesOutput();
        }

        public static IOutputProvider GetScruffDecompileOutputProvider()
        {
            return new DecompileScruffOutput();
        }

        public static IOutputProvider GetScruffHeaderOutputProvider()
        {
            return new DecompileScruffHeaderOutput();
        }
    }
}