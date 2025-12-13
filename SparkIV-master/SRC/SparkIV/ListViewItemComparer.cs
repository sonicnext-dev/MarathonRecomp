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
using System.Collections;
using System.Windows.Forms;
using RageLib.FileSystem.Common;

namespace SparkIV
{
    internal class ListViewItemComparer : IComparer
    {
        private readonly int _column;
        private readonly bool _descending;

        public ListViewItemComparer(bool descending)
        {
            // For size comparision
            _column = -1;
            _descending = descending;
        }

        public ListViewItemComparer(int column, bool descending)
        {
            _column = column;
            _descending = descending;
        }

        public int Compare(object x, object y)
        {
            if (x == null || y == null)
            {
                return 0;
            }

            try
            {
                if (_column > -1)
                {
                    int returnVal = String.Compare(((ListViewItem) x).SubItems[_column].Text,
                                                   ((ListViewItem) y).SubItems[_column].Text);

                    returnVal *= _descending ? -1 : 1;
                    return returnVal;
                }
                else
                {
                    File fileX = ((ListViewItem) x).Tag as File;
                    File fileY = ((ListViewItem) y).Tag as File;
                    return (fileX.Size - fileY.Size)*(_descending ? -1 : 1);
                }
            }
            catch
            {
                return 0;
            }
        }
    }
}