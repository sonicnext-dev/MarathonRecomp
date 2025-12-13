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

using System.IO;
using RageLib.Common;
using RageLib.Common.Resources;

namespace RageLib.HyperText.Resource
{
    internal class HtmlRenderState : IFileAccess
    {
        public HtmlAttributeValue Display { get; set; }
        public float Width { get; set; }
        public float Height { get; set; }
        private float _fC;
        private float _f10;
        private byte[] _f14;
        private float _f18;
        private float _f1C;
        public uint BackgroundColor { get; set; }
        public uint BackgroundImageOffset { get; set; }
        private uint _f28h;
        private uint _f28l;
        public HtmlAttributeValue BackgroundRepeat { get; set; }
        public uint Color { get; set; }
        public HtmlAttributeValue HorizontalAlign { get; set; }
        public HtmlAttributeValue VerticalAlign { get; set; }
        public HtmlAttributeValue TextDecoration { get; set; }
        private uint _f44;
        public HtmlAttributeValue FontSize { get; set; }
        public int FontStyle { get; set; }
        public int FontWeight { get; set; }
        private uint _f54;
        public uint BorderBottomColor { get; set; }
        public HtmlAttributeValue BorderBottomStyle { get; set; }
        public float BorderBottomWidth { get; set; }
        public uint BorderLeftColor { get; set; }
        public HtmlAttributeValue BorderLeftStyle { get; set; }
        public float BorderLeftWidth { get; set; }
        public uint BorderRightColor { get; set; }
        public HtmlAttributeValue BorderRightStyle { get; set; }
        public float BorderRightWidth { get; set; }
        public uint BorderTopColor { get; set; }
        public HtmlAttributeValue BorderTopStyle { get; set; }
        public float BorderTopWidth { get; set; }
        public float MarginBottom { get; set; }
        public float MarginLeft { get; set; }
        public float MarginRight { get; set; }
        public float MarginTop { get; set; }
        public float PaddingBottom { get; set; }
        public float PaddingLeft { get; set; }
        public float PaddingRight { get; set; }
        public float PaddingTop { get; set; }
        public float CellPadding { get; set; }
        public float CellSpacing { get; set; }
        public int ColSpan { get; set; }
        public int RowSpan { get; set; }
        public byte HasBackground { get; set; }
        private byte _fB9;
        private byte[] _fBA;
        public uint ALinkColor { get; set; }
        private int _fC0;

        public TextureInfo BackgroundImageTextureInfo { get; private set; }

        public HtmlRenderState()
        {
        }

        public HtmlRenderState(BinaryReader br)
        {
            Read(br);
        }

        #region Implementation of IFileAccess

        public void Read(BinaryReader br)
        {
            Display = (HtmlAttributeValue) br.ReadInt32();
            Width = br.ReadSingle();
            Height = br.ReadSingle();
            _fC = br.ReadSingle();
            _f10 = br.ReadSingle();
            _f14 = new byte[] { br.ReadByte(), br.ReadByte(), br.ReadByte(), br.ReadByte() };
            _f18 = br.ReadSingle();
            _f1C = br.ReadSingle();
            BackgroundColor = br.ReadUInt32();
            BackgroundImageOffset = ResourceUtil.ReadOffset(br);
            _f28h = br.ReadUInt32();
            _f28l = br.ReadUInt32();
            BackgroundRepeat = (HtmlAttributeValue) br.ReadInt32();
            Color = br.ReadUInt32();
            HorizontalAlign = (HtmlAttributeValue) br.ReadInt32();
            VerticalAlign = (HtmlAttributeValue) br.ReadInt32();
            TextDecoration = (HtmlAttributeValue) br.ReadInt32();
            _f44 = br.ReadUInt32();
            FontSize = (HtmlAttributeValue) br.ReadInt32();
            FontStyle = br.ReadInt32();
            FontWeight = br.ReadInt32();
            _f54 = br.ReadUInt32();
            BorderBottomColor = br.ReadUInt32();
            BorderBottomStyle = (HtmlAttributeValue) br.ReadInt32();
            BorderBottomWidth = br.ReadSingle();
            BorderLeftColor = br.ReadUInt32();
            BorderLeftStyle = (HtmlAttributeValue) br.ReadInt32();
            BorderLeftWidth = br.ReadSingle();
            BorderRightColor = br.ReadUInt32();
            BorderRightStyle = (HtmlAttributeValue) br.ReadInt32();
            BorderRightWidth = br.ReadSingle();
            BorderTopColor = br.ReadUInt32();
            BorderTopStyle = (HtmlAttributeValue) br.ReadInt32();
            BorderTopWidth = br.ReadSingle();
            MarginBottom = br.ReadSingle();
            MarginLeft = br.ReadSingle();
            MarginRight = br.ReadSingle();
            MarginTop = br.ReadSingle();
            PaddingBottom = br.ReadSingle();
            PaddingLeft = br.ReadSingle();
            PaddingRight = br.ReadSingle();
            PaddingTop = br.ReadSingle();
            CellPadding = br.ReadSingle();
            CellSpacing = br.ReadSingle();
            ColSpan = br.ReadInt32();
            RowSpan = br.ReadInt32();
            HasBackground = br.ReadByte();
            _fB9 = br.ReadByte();
            _fBA = new byte[] {br.ReadByte(), br.ReadByte()};
            ALinkColor = br.ReadUInt32();
            _fC0 = br.ReadInt32();

            if (BackgroundImageOffset != 0)
            {
                var offset = br.BaseStream.Position;
                
                br.BaseStream.Seek(BackgroundImageOffset, SeekOrigin.Begin);
                BackgroundImageTextureInfo = new TextureInfo(br);

                br.BaseStream.Seek(offset, SeekOrigin.Begin);
            }
        }

        public void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}