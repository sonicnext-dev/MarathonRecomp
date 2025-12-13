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

namespace RageLib.Scripting.Script
{
    static class LiteralFormatter
    {
        public static string FormatInteger(int value)
        {
            if (value > 0xFFFFFF || value < -0xFFFFFF)
            {
                return string.Format("0x{0:x}", value);
            }
            else
            {
                return value.ToString();
            }
        }

        public static string FormatFloat(float value)
        {
            var tempF = (float) value;
            if (Math.Round(tempF) == tempF)
            {
                return tempF + ".0f";
            }
            else
            {
                return tempF + "f";
            }
        }

        public static string FormatString(string value)
        {
            string temp = value;
            temp = temp.Replace("\\", "\\\\");
            temp = temp.Replace("\n", "\\n");
            temp = temp.Replace("\"", "\\\"");
            return "\"" + temp + "\"";
        }
    }
}