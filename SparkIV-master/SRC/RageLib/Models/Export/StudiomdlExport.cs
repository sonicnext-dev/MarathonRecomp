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
using System.Globalization;
using System.IO;
using RageLib.Common.ResourceTypes;
using RageLib.Models.Data;

namespace RageLib.Models.Export
{
    internal class StudiomdlExport : IExporter
    {
        private Dictionary<object, bool> _selectedNodes;

        #region Implementation of IExporter

        public string Name
        {
            get { return "Valve Studiomdl Format"; }
        }

        public string Extension
        {
            get { return "smd"; }
        }

        public void Export(ModelNode node, string filename)
        {
            _selectedNodes = new Dictionary<object, bool>();
            PrepareSelectedNodes(node, false);

            try
            {
                using (var fs = new FileStream(filename, FileMode.Create, FileAccess.Write))
                {
                    using (var sw = new StreamWriter(fs))
                    {
                        sw.WriteLine("version 1");
                        sw.WriteLine("nodes");

                        Skeleton skeleton = FindSkeleton(node);
                        if (skeleton != null)
                        {
                            ExportNodes(sw, skeleton.RootBone, null);
                        }
                        else
                        {
                            sw.WriteLine("0 \"root\" -1");
                        }
                        sw.WriteLine("end");

                        sw.WriteLine("skeleton");
                        sw.WriteLine("time 0");
                        if (skeleton != null)
                        {
                            ExportSkeleton(sw, skeleton.RootBone);
                        }
                        else
                        {
                            sw.WriteLine("0   0.0 0.0 0.0   0.0 0.0 0.0");
                        }
                        sw.WriteLine("end");

                        sw.WriteLine("triangles");
                        ExportModelNode(sw, node);
                        sw.WriteLine("end");
                    }
                }
            }
            finally
            {
                _selectedNodes = null;
            }
        }

        #endregion

        #region Helpers

        private static Skeleton FindSkeleton(ModelNode node)
        {
            Drawable drawable = node.DataModel as Drawable;
            if (drawable != null)
            {
                if (drawable.Skeleton != null)
                {
                    return drawable.Skeleton;
                }
            }
            else
            {
                foreach (var child in node.Children)
                {
                    return FindSkeleton(child);
                }
            }
            return null;
        }

        private void PrepareSelectedNodes(ModelNode node, bool parentNodeSelected)
        {
            bool nodeSelected = parentNodeSelected | node.Selected;
            _selectedNodes.Add(node.DataModel, nodeSelected);
            foreach (var child in node.Children)
            {
                PrepareSelectedNodes(child, nodeSelected);
            }
        }

        private void ExportModelNode(TextWriter sw, ModelNode node)
        {
            Drawable drawable = node.DataModel as Drawable;
            if (drawable != null)
            {
                ExportDrawable(sw, drawable);
            }
            else
            {
                foreach (var child in node.Children)
                {
                    ExportModelNode(sw, child);
                }
            }
        }

        private static string F(float f)
        {
            return f.ToString("0.000000", CultureInfo.InvariantCulture);
        }

        private void ExportDrawable(TextWriter sw, Drawable drawable)
        {
            foreach (var model in drawable.Models)
            {
                ExportModel(sw, model, drawable);
            }
        }

        private void ExportModel(TextWriter sw, Model model, Drawable drawable)
        {
            foreach (var geometry in model.Geometries)
            {
                foreach (var mesh in geometry.Meshes)
                {
                    if (_selectedNodes[mesh])
                    {

                        Vertex[] vertices = mesh.DecodeVertexData();
                        ushort[] indices = mesh.DecodeIndexData();
                        string materialName = "material_" + mesh.MaterialIndex + "_" +
                                              drawable.Materials[mesh.MaterialIndex].ShaderName;

                        for (int i = 0; i < mesh.FaceCount; i++)
                        {
                            Vertex v1 = vertices[indices[i*3 + 0]];
                            Vertex v2 = vertices[indices[i*3 + 1]];
                            Vertex v3 = vertices[indices[i*3 + 2]];

                            sw.WriteLine(materialName);
                            ExportVertex(sw, mesh, v1);
                            ExportVertex(sw, mesh, v2);
                            ExportVertex(sw, mesh, v3);
                        }
                    }
                }
            }
        }

        private static void ExportVertex(TextWriter sw, Mesh m, Vertex v)
        {
            var boneIndex = m.VertexHasBlendInfo ? v.BlendIndices[0] : 0;
            var normal = m.VertexHasNormal ? v.Normal : new Vector3();
            var uv = m.VertexHasTexture ? v.TextureCoordinates[0] : new Vector2();
            var position = v.Position;

            sw.WriteLine("{0}   {1} {2} {3}   {4} {5} {6}   {7} {8}   1 0   1.000000", boneIndex,
                         F(position.X), F(position.Y), F(position.Z),
                         F(normal.X), F(normal.Y), F(normal.Z),
                         F(uv.X), F(uv.Y));
        }

        private static void ExportNodes(TextWriter sw, Bone bone, Bone parentBone)
        {
            sw.WriteLine("{0} \"{1}\" {2}", bone.Index, bone.Name, parentBone == null ? -1 : parentBone.Index);

            foreach (var childBone in bone.Children)
            {
                ExportNodes(sw, childBone, bone);
            }
        }

        private static void ExportSkeleton(TextWriter sw, Bone bone)
        {
            sw.WriteLine("{0}   {1} {2} {3}   {4} {5} {6}", bone.Index,
                         F(bone.Position.X), F(bone.Position.Y), F(bone.Position.Z),
                         F(bone.Rotation.X), F(bone.Rotation.Y), F(bone.Rotation.Z));

            foreach (var childBone in bone.Children)
            {
                ExportSkeleton(sw, childBone);
            }
        }

        #endregion
    }
}
