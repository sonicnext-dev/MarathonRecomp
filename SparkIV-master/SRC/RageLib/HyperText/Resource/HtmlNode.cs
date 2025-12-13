/**********************************************************************\

 RageLib - HyperText
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
using System.IO;
using System.Text;
using RageLib.Common;
using RageLib.Common.Resources;
using RageLib.Common.ResourceTypes;

namespace RageLib.HyperText.Resource
{
    class HtmlNode : IFileAccess
    {
        private uint VTable { get; set; }
        public HtmlNodeType NodeType { get; set; }
        private uint ParentNodeOffset { get; set; }
        public HtmlNode ParentNode { get; private set; }
        public PtrCollection<HtmlNode> ChildNodes { get; private set; }
        public HtmlRenderState RenderState { get; set; }

        // For ElementNodes
        public HtmlTag Tag { get; set; }
        private uint _fDC;
        private SimpleCollection<byte> _linkAddress;
        private int _fE8;
        private int _fEC;

        // For DataNodes
        public PtrString Data { get; private set; }

        public HtmlNode()
        {
        }

        public HtmlNode(BinaryReader br)
        {
            Read(br);
        }

        public string LinkAddress
        {
            get
            {
                StringBuilder sb = new StringBuilder();
                foreach (var b in _linkAddress)
                {
                    sb.Append((char) b);
                }
                return sb.ToString();
            }
        }

        public override string ToString()
        {
            if (NodeType == HtmlNodeType.HtmlDataNode)
            {
                return Data.Value;
            }
            else
            {
                return Tag.ToString();
            }
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            VTable = br.ReadUInt32();
            NodeType = (HtmlNodeType)br.ReadInt32();
            ParentNodeOffset = ResourceUtil.ReadOffset(br);
            
            ChildNodes = new PtrCollection<HtmlNode>(br);
            foreach (var node in ChildNodes)
            {
                node.ParentNode = this;
            }

            RenderState = new HtmlRenderState(br);

            if (NodeType != HtmlNodeType.HtmlDataNode)
            {
                Tag = (HtmlTag)br.ReadInt32();
                _fDC = br.ReadUInt32();
                _linkAddress = new SimpleCollection<byte>(br, reader => reader.ReadByte());
                if (NodeType == HtmlNodeType.HtmlTableElementNode)
                {
                    _fE8 = br.ReadInt32();
                    _fEC = br.ReadInt32();
                }
            }
            else
            {
                Data = new PtrString(br);
            }
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion

    }
}
