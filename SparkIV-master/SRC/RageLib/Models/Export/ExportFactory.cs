/**********************************************************************\

 RageLib - Models
 Copyright (C) 2009  Arushan/Aru <oneforaru at gmail.com>

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

using System.Text;

namespace RageLib.Models.Export
{
    internal static class ExportFactory
    {
        private static readonly IExporter[] exporterTypes;

        static ExportFactory()
        {
            exporterTypes = new[]
                               {
                                   new StudiomdlExport(),
                               };
        }

        public static string GenerateFilterString()
        {
            var sb = new StringBuilder();
            foreach (var type in exporterTypes)
            {
                if (sb.Length > 0)
                {
                    sb.Append("|");
                }
                sb.AppendFormat("{0} (*.{1})|*.{1}", type.Name, type.Extension);
            }
            return sb.ToString();
        }

        public static IExporter GetExporter( int exporterIndex )
        {
            return exporterTypes[exporterIndex];
        }
    }
}
