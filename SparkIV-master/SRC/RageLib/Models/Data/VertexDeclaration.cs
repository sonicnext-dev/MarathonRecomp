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

using System;
using System.Collections.Generic;

namespace RageLib.Models.Data
{
    public class VertexDeclaration
    {
        public VertexElement[] Elements { get; private set; }

        internal VertexDeclaration(Resource.Models.VertexDeclaration declaration)
        {
            if (declaration.AlterateDecoder == 1)
            {
                throw new Exception("Don't know how to handle alterate decoder vertex declaration");
            }

            // Lets convert the RAGE VertexElement declarations to a more DirectX like one...

            var rageElements = declaration.DecodeAsVertexElements();
            var elements = new List<VertexElement>();

            int streamIndex = rageElements[0].StreamIndex;
            int offsetInStream = 0;

            int[] typeMapping = {0, 15, 0, 16, 0, 1, 2, 3, 5, 4, 14, 0, 0, 0, 0, 0};
            int[] usageMapping = {0, 9, 3, 7, 6, 5, 1, 2, 10, 0, 0, 0};

            foreach (var rageEl in rageElements)
            {
                if (rageEl.StreamIndex != streamIndex)
                {
                    streamIndex = rageEl.StreamIndex;
                    offsetInStream = 0;
                }

                var el = new VertexElement()
                             {
                                 Stream = streamIndex,
                                 Type = (VertexElementType) typeMapping[(int) rageEl.Type],
                                 Usage = (VertexElementUsage) usageMapping[(int) rageEl.Usage],
                                 Size = rageEl.Size,
                                 Offset = offsetInStream,
                                 Method = VertexElementMethod.Default,
                                 UsageIndex = rageEl.UsageIndex,
                             };
                
                offsetInStream += rageEl.Size;

                elements.Add(el);
            }

            elements.Add(VertexElement.End);

            Elements = elements.ToArray();
        }
    }
}