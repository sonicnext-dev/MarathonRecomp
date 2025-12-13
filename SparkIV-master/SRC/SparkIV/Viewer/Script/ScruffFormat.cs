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

namespace SparkIV.Viewer.Script
{
    class ScruffFormat : Manoli.Utils.CSharpFormat.CLikeFormat
    {
        /// <summary>
        /// The list of C# keywords.
        /// </summary>
        protected override string Keywords
        {
            get
            {
                return "void function auto return if else while switch case default break float int";
            }
        }

        /// <summary>
        /// The list of C# preprocessors.
        /// </summary>
        protected override string Preprocessors
        {
            get
            {
                return "";
            }
        }
    }
}