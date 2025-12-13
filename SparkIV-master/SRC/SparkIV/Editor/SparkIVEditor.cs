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

using System.IO;
using RageLib.FileSystem;
using RageLib.FileSystem.Common;
using File=RageLib.FileSystem.Common.File;

namespace SparkIV.Editor
{
    class SparkIVEditor : IEditor
    {
        public void LaunchEditor(FileSystem fs, File file)
        {
            if (fs is RealFileSystem)
            {
                var form = new MainForm();
                form.Show();

                DirectoryInfo parent = new DirectoryInfo((fs as RealFileSystem).RealDirectory).Parent;
                string archiveFilename = parent == null ? file.FullName : Path.Combine(parent.FullName, file.FullName);
                form.OpenFile(archiveFilename, null);
            }

        }
    }
}
