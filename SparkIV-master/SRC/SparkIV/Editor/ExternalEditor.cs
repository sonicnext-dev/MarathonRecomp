/**********************************************************************\

 Spark IV
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
using System.IO;
using Microsoft.Win32;
using RageLib.FileSystem;
using RageLib.FileSystem.Common;
using File=RageLib.FileSystem.Common.File;

namespace SparkIV.Editor
{
    class ExternalEditor : IEditor, IDynamicEditor
    {
        public void LaunchEditor(FileSystem fs, File file)
        {
            if (fs is RealFileSystem)
            {
                // We'll edit RealFileSystems on the spot... no memory caching
                // Some of the files are pretty big...

                DirectoryInfo parent = new DirectoryInfo((fs as RealFileSystem).RealDirectory).Parent;
                string filename = parent == null ? file.FullName : Path.Combine(parent.FullName, file.FullName);

                var info = new ProcessStartInfo(filename);
                info.UseShellExecute = true;

                var p = Process.Start(info);
                if (p != null)
                {
                    p.WaitForExit();
                }
            }
            else
            {
                // Export the file to a temporary file and load it up

                string tempFileName = Path.Combine(Path.GetTempPath(), file.Name);
                System.IO.File.WriteAllBytes(tempFileName, file.GetData());

                var info = new ProcessStartInfo(tempFileName);
                info.UseShellExecute = true;

                var p = Process.Start(info);
                if (p != null)
                {
                    p.WaitForExit();

                    if (p.ExitCode == 0)
                    {
                        var data = System.IO.File.ReadAllBytes(tempFileName);
                        file.SetData(data);
                    }
                }

            }
        }

        public bool SupportsExtension(string extension)
        {
            try
            {
                var key = Registry.ClassesRoot.OpenSubKey("." + extension);
                
                if (key == null) 
                {
                    return false;
                }
                
                var defaultValue = key.GetValue("").ToString();
                
                if (defaultValue == null)
                {
                    return false;
                }

                var shellOpenKey = Registry.ClassesRoot.OpenSubKey(defaultValue + @"\shell\open");

                return shellOpenKey != null;
            }
            catch
            {
                return false;
            }
        }
    }
}
