/**********************************************************************\

 RageLib - HyperText
 Copyright (C) 2008-2009  Arushan/Aru <oneforaru at gmail.com>

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

using System.IO;
using RageLib.Textures;
using File=RageLib.HyperText.Resource.File;
using Stream=System.IO.Stream;

namespace RageLib.HyperText
{
    public class HyperTextFile
    {
        internal File File { get; private set; }

        public void Open(string filename)
        {
            File = new File();
            File.Open(filename);
        }

        public void Open(Stream stream)
        {
            File = new File();
            File.Open(stream);
        }

        public TextureFile EmbeddedTextureFile
        {
            get { return File.Data.TextureDictionary; }
        }

        public void WriteHTML(TextWriter writer)
        {
            HyperTextExport.Export(File.Data.RootElement, writer);
        }

    }
}
