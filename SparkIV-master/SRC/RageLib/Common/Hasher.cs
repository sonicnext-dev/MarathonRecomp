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

namespace RageLib.Common
{
    public static class Hasher
    {
        public static uint Hash(string str)
        {
            uint value = 0, temp;
            var index = 0;
            var quoted = false;

            if (str[index] == '"')
            {
                quoted = true;
                index++;
            }

            str = str.ToLower();

            for (; index < str.Length; index++)
            {
                var v = str[index];

                if (quoted && (v == '"')) break;

                if (v == '\\')
                    v = '/';

                temp = v;
                temp = temp + value;
                value = temp << 10;
                temp += value;
                value = temp >> 6;
                value = value ^ temp;
            }

            temp = value << 3;
            temp = value + temp;
            var temp2 = temp >> 11;
            temp = temp2 ^ temp;
            temp2 = temp << 15;

            value = temp2 + temp;

            if (value < 2) value += 2;

            return value;
        }
    }
}