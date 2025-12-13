/**********************************************************************\

 RageLib - Audio
 Copyright (C) 2009  DerPlaya78

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
using System.Windows.Forms;
using System.Collections;

namespace RageLib.Audio
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
                    int returnVal;
                    ListViewItem lvix = ((ListViewItem) x);
                    ListViewItem lviy = ((ListViewItem) y);

                    if (lvix.SubItems[_column].Tag is TimeSpan)
                    {
                        TimeSpan tx = (TimeSpan) lvix.SubItems[_column].Tag;
                        TimeSpan ty = (TimeSpan) lviy.SubItems[_column].Tag;
                        returnVal = tx.CompareTo(ty);
                    }
                    else if (lvix.SubItems[_column].Tag is int)
                    {
                        returnVal = ((int) lvix.SubItems[_column].Tag).CompareTo((int) lviy.SubItems[_column].Tag);
                    }
                    else
                    {
                        returnVal = String.Compare(lvix.SubItems[_column].Text, lviy.SubItems[_column].Text);
                    }

                    returnVal *= _descending ? -1 : 1;
                    return returnVal;
                }
                return 0;
            }
            catch
            {
                return 0;
            }
        }
    }
}
