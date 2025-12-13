/**********************************************************************\

 RageLib
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

using System.Collections.Generic;
using System.Windows.Media.Media3D;

namespace RageLib.Models
{
    public class ModelNode
    {
        public List<ModelNode> Children { get; set; }
        public string Name { get; set; }
        internal Model3D Model3D { get; set; }
        public object DataModel { get; set; }
        public bool NoCount { get; set; }
        public bool Selected { get; set; }

        public bool IsAnyNodeSelected()
        {
            if (Selected)
            {
                return true;
            }

            foreach (var node in Children)
            {
                if (node.IsAnyNodeSelected())
                {
                    return true;}
            }

            return false;
        }
        
        public ModelNode()
        {
            Children = new List<ModelNode>();
        }
    }
}
