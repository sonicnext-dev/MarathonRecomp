/**********************************************************************\

 Scruff -- A Rage Script File Decompiler/Disassembler
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
using System.IO;
using System.Text;
using RageLib.Common;
using RageLib.Scripting;

namespace Scruff
{
    class Program
    {
        private const string Version = "0.5.0";

        private class CodeFormatOptions
        {
            public string Param { get; set; }
            public CodeFormat Format { get; set; }
            public string Description { get; set; }

            public CodeFormatOptions(string param, CodeFormat format, string description)
            {
                Param = param;
                Format = format;
                Description = description;
            }
        }

        static int Main(string[] args)
        {
            string gtaPath = KeyUtil.FindGTADirectory();
            while (gtaPath == null)
            {
                Console.Error.WriteLine("ERROR");
                Console.Error.WriteLine("Could not find GTAIV directory. Please install GTAIV or copy EFLC.exe\n" +
                                    "to the same path as Scruff.");
                return 1;
            }

            byte[] key = KeyUtil.FindKey(gtaPath);
            if (key == null)
            {
                Console.Error.WriteLine("ERROR");
                Console.Error.WriteLine("Your EFLC.exe seems to be modified or is a newer version than this tool\n" +
                                        "supports. If it is a newer version, please check for an update of Scruff.\n" +
                                        "Scruff can not run without a supported EFLC.exe file.");
                return 1;
            }

            KeyStore.SetKeyLoader(() => key);


            CodeFormatOptions[] formats = 
                {
                    new CodeFormatOptions("d", CodeFormat.ScruffDecompile, "Default Scruff disassembly format"),
                    new CodeFormatOptions("h", CodeFormat.ScruffHeader, "Default Scruff header/local varibles/etc format"),
                    new CodeFormatOptions("hl", CodeFormat.FullDecompile, "High level C-like format"),
                    new CodeFormatOptions("hla", CodeFormat.FullDecompileAnnotate, "High level C-like format (annotated)"),
                    new CodeFormatOptions("ll", CodeFormat.Disassemble, "Low level raw assembly format"),
                    new CodeFormatOptions("cp", CodeFormat.CodePath, "Code path for the control-flow-analyzer (for debugging)"),
                };

            CodeFormat customFormat = CodeFormat.ScruffDecompile;
            bool defaultMode = true;
            string filename = null;
            string outputFilename = null;

            if (args.Length > 0)
            {
                var argsQueue = new Queue<string>(args);

                while (argsQueue.Count > 0)
                {
                    var arg = argsQueue.Dequeue();
                    if (arg.StartsWith("-"))
                    {
                        if (arg == "-o")
                        {
                            defaultMode = false;
                            outputFilename = argsQueue.Dequeue();
                        }
                        else
                        {
                            foreach (var format in formats)
                            {
                                if (arg == "-" + format.Param)
                                {
                                    defaultMode = false;
                                    customFormat = format.Format;
                                    break;
                                }
                            }                            
                        }
                    }
                    else
                    {
                        if (argsQueue.Count > 0)
                        {
                            break;
                        }
                        filename = arg;
                    }
                }
            }

            if (filename == null)
            {
                var formatParams = new StringBuilder();
                foreach (var format in formats)
                {
                    if (formatParams.Length > 0)
                    {
                        formatParams.Append("|");
                    }
                    formatParams.Append("-");
                    formatParams.Append(format.Param);
                }

                Console.Error.WriteLine("Scruff - A RAGE Script File Decompiler/Disassembler");
                Console.Error.WriteLine("v" + Version + " -- (c) 2008-2009, Aru <oneforaru at gmail dot com>");
                Console.Error.WriteLine();
                Console.Error.WriteLine(string.Format("Usage: scruff [{0}] [-o filename.sca] filename.sco", formatParams));
                Console.Error.WriteLine();
                Console.Error.WriteLine("By default, will generate filename.sca (-d) and filename.sch (-h)");
                Console.Error.WriteLine("If output file is specified, only filename.sca will be generated.");
                Console.Error.WriteLine("If format specified without output filename, will dump to console (stdout).");
                Console.Error.WriteLine();
                Console.Error.WriteLine("For custom options, use:");
                Console.Error.WriteLine("    -{0,-5} {1}", "o", "Saves the result to a specified file.");
                foreach (var format in formats)
                {
                    Console.Error.WriteLine("    -{0,-5} {1}", format.Param, format.Description);
                }
                Console.Error.WriteLine();
                /*
                Console.Error.WriteLine("Press any key to exit");
                Console.ReadKey();
                 */
                return 1;
            }

            var file = new ScriptFile();

            try
            {
                file.Open(filename);
            }
            catch
            {
                Console.Error.WriteLine("Invalid input file -- not a valid script.");
                /*
                Console.ReadKey();
                 */
                return 1;
            }

            if (defaultMode)
            {
                using (var fs = File.OpenWrite(Path.ChangeExtension(filename, "sca")))
                {
                    var sw = new StreamWriter(fs);
                    OutputCode(file, CodeFormat.ScruffDecompile, sw);
                    sw.Flush();
                }

                using (var fs = File.OpenWrite(Path.ChangeExtension(filename, "sch")))
                {
                    var sw = new StreamWriter(fs);
                    OutputCode(file, CodeFormat.ScruffHeader, sw);
                    sw.Flush();
                }
            }
            else
            {
                if (outputFilename != null)
                {
                    using(var fs = File.OpenWrite(outputFilename))
                    {
                        var sw = new StreamWriter(fs);
                        OutputCode(file, customFormat, sw);
                        sw.Flush();
                    }
                }
                else
                {
                    OutputCode(file, customFormat, Console.Out);                    
                }

            }

#if DEBUG
            Console.ReadLine();
#endif

            return 0;
        }

        private static void OutputCode(ScriptFile file, CodeFormat format, TextWriter writer)
        {
            writer.WriteLine("// Generated by Scruff v" + Version + ", (c) 2008, aru.");
            writer.WriteLine("// This disassembly/decompilation is licensed for research purposes only.");
            writer.WriteLine();

            file.GetCode(format, writer);
        }
    }
}
