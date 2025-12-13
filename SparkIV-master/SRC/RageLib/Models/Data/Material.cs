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

using System.Collections.Generic;
using RageLib.Models.Resource.Shaders;

namespace RageLib.Models.Data
{
    public class Material
    {
        public string ShaderName { get; private set; }
        public Dictionary<uint, MaterialParam> Parameters { get; private set; }

        internal Material(ShaderFx info)
        {
            ShaderName = info.ShaderName;

            Parameters = new Dictionary<uint, MaterialParam>(info.ShaderParamCount);
            foreach (var data in info.ShaderParams)
            {
                Parameters.Add((uint) data.Key, MaterialParam.Create((uint) data.Key, data.Value));
            }
        }
    }
}