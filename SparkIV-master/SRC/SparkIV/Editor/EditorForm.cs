/**********************************************************************\

 Spark IV - Textures
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

using System.Windows.Forms;

namespace SparkIV.Editor
{
    public partial class EditorForm : Form
    {
        private Control _viewControl;

        public EditorForm()
        {
            InitializeComponent();
        }

        public void SetFilename(string filename)
        {
            Text = "Spark IV - Edit - " + filename;
        }

        public void SetControl(Control control)
        {
            if (_viewControl != null)
            {
                Controls.Remove(_viewControl);
            }

            _viewControl = control;
            _viewControl.Parent = this;
            _viewControl.Dock = DockStyle.Fill;
            Controls.Add(_viewControl);
        }
    }
}
