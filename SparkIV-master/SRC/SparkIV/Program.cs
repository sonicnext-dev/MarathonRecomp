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

using System;
using System.IO;
using System.Windows.Forms;
using RageLib.Common;
using System.Reflection;

namespace SparkIV
{
    static class Program
    {

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            /*
            Version envVer = Environment.Version;
            if (envVer.Major <= 2 && envVer.Revision < 1435) // 2.0 SP1 (http://en.wikipedia.org/wiki/.NET_Framework_version_list)
            {
                MessageBox.Show("It appears that you are not running the latest version of the .NET Framework.\n\n" +
                    "SparkIV requires that you atleast have both .NET Framework 2.0 SP1 and 3.0 installed. " + 
                    "Alternatively an install of .NET Framework 3.5 will install both these components.",
                    "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
             */

            Application.Run(new MainForm());
        }
    }
}

