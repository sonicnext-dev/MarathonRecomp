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
using System.Windows.Forms;

namespace SparkIV.Config
{
    static class SparkIVConfig
    {
        public static readonly Value.Config Instance;

        static SparkIVConfig()
        {
            var appPath = Path.GetDirectoryName(Application.ExecutablePath);
            Instance = XmlPersister.Load<Value.Config>(Path.Combine(appPath, "SparkIV.Config.xml"));
        }
    }
}
