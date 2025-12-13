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
using System.Xml.Serialization;

namespace SparkIV.Config
{
    internal static class XmlPersister
    {
        public static T Load<T>(string file) where T : class
        {
            T value = null;
            var fs = new FileStream(file, FileMode.Open, FileAccess.Read);
            try
            {
                var xs = new XmlSerializer(typeof (T));
                value = xs.Deserialize(fs) as T;
            }
            catch (Exception e)
            {
                MessageBox.Show("Error: " + e.Message);
            }
            finally
            {
                fs.Close();
            }
            return value;
        }

        public static void Save<T>(string file, T value) where T : class
        {
            var fs = new FileStream(file, FileMode.Create, FileAccess.Write);
            try
            {
                var xs = new XmlSerializer(typeof (T));
                xs.Serialize(fs, value);
            }
            catch (Exception e)
            {
                MessageBox.Show("Error: " + e.Message);
            }
            finally
            {
                fs.Close();
            }
        }
    }
}