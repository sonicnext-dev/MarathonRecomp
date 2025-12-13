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
using System.Collections.Generic;
using System.Windows.Forms;
using RageLib.FileSystem.Common;
using SparkIV.Config;

namespace SparkIV.Viewer
{
    static class Viewers
    {
        static readonly Dictionary<string, IViewer> _viewers = new Dictionary<string, IViewer>();

        static Viewers()
        {
            foreach (var viewer in SparkIVConfig.Instance.Viewers)
            {
                var viewerType = Type.GetType(viewer.Type);
                if (viewerType != null)
                {
                    var viewerObject = Activator.CreateInstance(viewerType);

                    if (viewerObject is IViewer)
                    {
                        var extensions = viewer.Extension.Split(',');

                        foreach (var s in extensions)
                        {
                            _viewers.Add(s, viewerObject as IViewer);
                        }
                    }
                }
            }
        }

        public static bool HasViewer(File file)
        {
            var fileName = file.Name;
            var extension = fileName.Substring(fileName.LastIndexOf('.') + 1);

            return _viewers.ContainsKey(extension);
        }

        public static Control GetControl(File file)
        {
            var fileName = file.Name;
            var extension = fileName.Substring(fileName.LastIndexOf('.') + 1);

            if (_viewers.ContainsKey(extension))
            {
                var control = _viewers[extension].GetView(file);
                if (control != null)
                {
                    return control;
                }
            }

            return null;
        }
    }
}
