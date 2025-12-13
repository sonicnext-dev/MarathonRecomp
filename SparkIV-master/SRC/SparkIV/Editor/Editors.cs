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
using RageLib.FileSystem.Common;
using SparkIV.Config;

namespace SparkIV.Editor
{
    class Editors
    {
        static readonly Dictionary<string, IEditor> _editors = new Dictionary<string, IEditor>();

        static Editors()
        {
            foreach (var editor in SparkIVConfig.Instance.Editors)
            {
                var editorType = Type.GetType(editor.Type);
                if (editorType != null)
                {
                    var editorObject = Activator.CreateInstance(editorType);

                    if (editorObject is IEditor)
                    {
                        var extensions = editor.Extension.Split(',');

                        foreach (var s in extensions)
                        {
                            _editors.Add(s, editorObject as IEditor);
                        }
                    }
                }
            }
        }

        public static bool HasEditor(File file)
        {
            var fileName = file.Name;
            var extension = fileName.Substring(fileName.LastIndexOf('.') + 1);
            
            bool hasEditor = _editors.ContainsKey(extension);

            if (!hasEditor && _editors.ContainsKey(""))
            {
                var dynamicEditor = _editors[""] as IDynamicEditor;
                if (dynamicEditor != null)
                {
                    hasEditor = dynamicEditor.SupportsExtension(extension);
                }
            }

            return hasEditor;
        }

        public static void LaunchEditor(FileSystem fs, File file)
        {
            var fileName = file.Name;
            var extension = fileName.Substring(fileName.LastIndexOf('.') + 1);

            if (_editors.ContainsKey(extension))
            {
                var editor = _editors[extension];
                editor.LaunchEditor(fs, file);
            }
            else
            {
                var editor = _editors[""];
                if (editor != null)
                {
                    editor.LaunchEditor(fs, file);
                }
            }
        }

    }
}
