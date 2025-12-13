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
using RageLib.HyperText.Resource;

namespace RageLib.HyperText
{
    static class HyperTextExport
    {
        public static void Export(HtmlNode rootNode, TextWriter writer)
        {
            ExportNode(rootNode, writer, "");
        }

        private static void ExportNode(HtmlNode node, TextWriter writer, string indent)
        {
            bool emptyNode = node.ChildNodes.Count == 0;
            bool dataNode = node.Data != null;

            if (dataNode)
            {
                string value = node.Data.Value;
                value = value.Replace("&", "&amp;");
                value = value.Replace("<", "&lt;");
                value = value.Replace(">", "&gt;");
                writer.Write("{0}", value);
            }
            else
            {
                string tagName = GetTagName(node.Tag);
                string attributes = GetAttributes(node);

                if (emptyNode && node.Tag != HtmlTag.Style)
                {
                    //writer.WriteLine("{0}<{1} {2}/>", indent, tagName, attributes);
                    writer.Write("<{0} {1}/>", tagName, attributes);
                }
                else if (node.Tag == HtmlTag.Style)
                {
                    writer.WriteLine("{0}<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">", indent);
                    writer.WriteLine("{0}<style>", indent);
                    writer.WriteLine(indent + "   body { font-family: Arial; }");
                    writer.WriteLine(indent + "   a:link { text-decoration: none; }");
                    writer.WriteLine("{0}</style>", indent);
                }
                else
                {
                    //writer.WriteLine("{0}<{1}{2}>", indent, tagName, attributes);
                    writer.Write("<{0}{1}>", tagName, attributes);
                    foreach (var childNode in node.ChildNodes)
                    {
                        ExportNode(childNode, writer, indent + "   ");
                    }
                    //writer.WriteLine("{0}</{1}>", indent, tagName);
                    writer.Write("</{0}>", tagName);
                }
            }

        }

        private static string GetAttributes(HtmlNode node)
        {
            Dictionary<string, string> attributes = new Dictionary<string, string>();
            Dictionary<string, string> styleAttributes = new Dictionary<string, string>();

            HtmlRenderState state = node.RenderState;

            if (node.Tag == HtmlTag.Html || node.Tag == HtmlTag.Head ||
                    node.Tag == HtmlTag.Title || node.Tag == HtmlTag.Style)
            {
                return "";
            }

            // Write out style attributes
            if (state.HasBackground == 1)
            {
                if (state.BackgroundImageOffset == 0)
                {
                    styleAttributes.Add("background-color", GetColor(state.BackgroundColor));
                }
                else
                {
                    styleAttributes.Add("background-image", "url(" + state.BackgroundImageTextureInfo.TextureName + ".png)");
                    styleAttributes.Add("background-repeat", GetAttributeValue(state.BackgroundRepeat));
                }
            }
            if (state.Width > -1)
            {
                if (node.NodeType == HtmlNodeType.HtmlTableNode || node.NodeType == HtmlNodeType.HtmlTableElementNode)
                {
                    attributes.Add("width", state.Width.ToString());
                }
                else
                {
                    styleAttributes.Add("width", GetSize(state.Width));
                }
            }
            if (state.Height > -1)
            {
                if (node.NodeType == HtmlNodeType.HtmlTableNode || node.NodeType == HtmlNodeType.HtmlTableElementNode)
                {
                    attributes.Add("height", state.Height.ToString());
                }
                else
                {
                    styleAttributes.Add("height", GetSize(state.Height));
                }
            }

            styleAttributes.Add("margin-left", GetSize(state.MarginLeft));
            styleAttributes.Add("margin-right", GetSize(state.MarginRight));
            styleAttributes.Add("margin-top", GetSize(state.MarginTop));
            styleAttributes.Add("margin-bottom", GetSize(state.MarginBottom));

            styleAttributes.Add("padding-left", GetSize(state.PaddingLeft));
            styleAttributes.Add("padding-right", GetSize(state.PaddingRight));
            styleAttributes.Add("padding-top", GetSize(state.PaddingTop));
            styleAttributes.Add("padding-bottom", GetSize(state.PaddingBottom));

            styleAttributes.Add("border-left", GetAttributeValue(state.BorderLeftStyle) + " " + GetSize(state.BorderLeftWidth) + " " + GetColor(state.BorderLeftColor));
            styleAttributes.Add("border-right", GetAttributeValue(state.BorderRightStyle) + " " + GetSize(state.BorderRightWidth) + " " + GetColor(state.BorderRightColor));
            styleAttributes.Add("border-top", GetAttributeValue(state.BorderTopStyle) + " " + GetSize(state.BorderTopWidth) + " " + GetColor(state.BorderTopColor));
            styleAttributes.Add("border-bottom", GetAttributeValue(state.BorderBottomStyle) + " " + GetSize(state.BorderBottomWidth) + " " + GetColor(state.BorderBottomColor));

            styleAttributes.Add("text-decoration", GetAttributeValue(state.TextDecoration));
            styleAttributes.Add("font-size", GetAttributeValue(state.FontSize));
            styleAttributes.Add("display", GetAttributeValue(state.Display));
            styleAttributes.Add("color", GetColor(state.Color));

            if (node.NodeType == HtmlNodeType.HtmlTableNode)
            {
                attributes.Add("cellpadding", state.CellPadding.ToString());
                attributes.Add("cellspacing", state.CellSpacing.ToString());
            }
            else if (node.NodeType == HtmlNodeType.HtmlTableElementNode)
            {
                if (state.ColSpan > 1)
                {
                    attributes.Add("colspan", state.ColSpan.ToString());                    
                }
                if (state.RowSpan > 1)
                {
                    attributes.Add("rowspan", state.RowSpan.ToString());
                }

                if ((int)state.VerticalAlign > -1)
                {
                    attributes.Add("valign", GetAttributeValue(state.VerticalAlign));
                }
                if ((int)state.HorizontalAlign > -1)
                {
                    attributes.Add("align", GetAttributeValue(state.HorizontalAlign));
                }
            }
            else
            {
                if ((int)state.VerticalAlign > -1)
                {
                    styleAttributes.Add("vertical-align", GetAttributeValue(state.VerticalAlign));
                }
                if ((int)state.HorizontalAlign > -1)
                {
                    styleAttributes.Add("text-align", GetAttributeValue(state.HorizontalAlign));
                }
            }
            
            if (node.Tag == HtmlTag.A)
            {
                attributes.Add("href", "#");
                attributes.Add("onclick", "alert('Link to: " + node.LinkAddress + "');");
            }
            else if (node.Tag == HtmlTag.Img)
            {
                string path = node.LinkAddress;
                path = path.Substring(0, path.Length - Path.GetExtension(path).Length);
                attributes.Add("src", path + ".png");
            }
            
            if (styleAttributes.Count > 0)
            {
                StringBuilder sbStyle = new StringBuilder();
                foreach (var pair in styleAttributes)
                {
                    sbStyle.AppendFormat("{0}: {1}; ", pair.Key, pair.Value);
                }
                attributes.Add("style", sbStyle.ToString());
            }

            StringBuilder sbAttributes = new StringBuilder();
            foreach (var pair in attributes)
            {
                sbAttributes.AppendFormat(" {0}=\"{1}\"", pair.Key, pair.Value);
            }

            return sbAttributes.ToString();
        }

        private static string GetColor(uint color)
        {
            return "#" + (color & 0xFFFFFF).ToString("x6");
        }

        private static string GetSize(float size)
        {
            return size + "px";
        }

        private static string GetAttributeValue(HtmlAttributeValue value)
        {
            string valueName = null;

            switch (value)
            {
                case HtmlAttributeValue.Left:
                    valueName = "left";
                    break;
                case HtmlAttributeValue.Right:
                    valueName = "right";
                    break;
                case HtmlAttributeValue.Center:
                    valueName = "center";
                    break;
                case HtmlAttributeValue.Justify:
                    valueName = "justify";
                    break;
                case HtmlAttributeValue.Top:
                    valueName = "top";
                    break;
                case HtmlAttributeValue.Bottom:
                    valueName = "bottom";
                    break;
                case HtmlAttributeValue.Middle:
                    valueName = "middle";
                    break;
                case HtmlAttributeValue.Inherit:
                    valueName = "inherit";
                    break;
                case HtmlAttributeValue.XXSmall:
                    //valueName = "xx-small";
                    valueName = "6px";
                    break;
                case HtmlAttributeValue.XSmall:
                    //valueName = "x-small";
                    valueName = "7px";
                    break;
                case HtmlAttributeValue.Small:
                    //valueName = "small";
                    valueName = "8px";
                    break;
                case HtmlAttributeValue.Medium:
                    //valueName = "medium";
                    valueName = "9px";
                    break;
                case HtmlAttributeValue.Large:
                    //valueName = "large";
                    valueName = "11px";
                    break;
                case HtmlAttributeValue.XLarge:
                    //valueName = "x-large";
                    valueName = "12px";
                    break;
                case HtmlAttributeValue.XXLarge:
                    //valueName = "xx-large";
                    valueName = "14px";
                    break;
                case HtmlAttributeValue.Block:
                    valueName = "block";
                    break;
                case HtmlAttributeValue.Inline:
                    valueName = "inline";
                    break;
                case HtmlAttributeValue.Table:
                    valueName = "table";
                    break;
                case HtmlAttributeValue.TableCell:
                    valueName = "table-cell";
                    break;
                case HtmlAttributeValue.None:
                    valueName = "none";
                    break;
                case HtmlAttributeValue.Solid:
                    valueName = "solid";
                    break;
                case HtmlAttributeValue.Underline:
                    valueName = "underline";
                    break;
                case HtmlAttributeValue.Overline:
                    valueName = "overline";
                    break;
                case HtmlAttributeValue.LineThrough:
                    valueName = "line-through";
                    break;
                case HtmlAttributeValue.Blink:
                    valueName = "blink";
                    break;
                case HtmlAttributeValue.Repeat:
                    valueName = "repeat";
                    break;
                case HtmlAttributeValue.NoRepeat:
                    valueName = "no-repeat";
                    break;
                case HtmlAttributeValue.RepeatX:
                    valueName = "repeat-x";
                    break;
                case HtmlAttributeValue.RepeatY:
                    valueName = "repeat-y";
                    break;
                case HtmlAttributeValue.Collapse:
                    valueName = "collapse";
                    break;
                case HtmlAttributeValue.Separate:
                    valueName = "seperate";
                    break;
                default:
                    throw new ArgumentOutOfRangeException("value");
            }

            return valueName;
        }

        private static string GetTagName(HtmlTag tag)
        {
            string tagName = null;
            switch(tag)
            {
                case HtmlTag.Html:
                    tagName = "html";
                    break;
                case HtmlTag.Title:
                    tagName = "title";
                    break;
                case HtmlTag.A:
                    tagName = "a";
                    break;
                case HtmlTag.Body:
                    tagName = "body";
                    break;
                case HtmlTag.B:
                    tagName = "b";
                    break;
                case HtmlTag.Br:
                    tagName = "br";
                    break;
                case HtmlTag.Center:
                    tagName = "center";
                    break;
                case HtmlTag.Code:
                    tagName = "code";
                    break;
                case HtmlTag.Dl:
                    tagName = "dl";
                    break;
                case HtmlTag.Dt:
                    tagName = "dt";
                    break;
                case HtmlTag.Dd:
                    tagName = "dd";
                    break;
                case HtmlTag.Div:
                    tagName = "div";
                    break;
                case HtmlTag.Embed:
                    tagName = "embed";
                    break;
                case HtmlTag.Em:
                    tagName = "em";
                    break;
                case HtmlTag.Head:
                    tagName = "head";
                    break;
                case HtmlTag.H1:
                    tagName = "h1";
                    break;
                case HtmlTag.H2:
                    tagName = "h2";
                    break;
                case HtmlTag.H3:
                    tagName = "h3";
                    break;
                case HtmlTag.H4:
                    tagName = "h4";
                    break;
                case HtmlTag.H5:
                    tagName = "h5";
                    break;
                case HtmlTag.H6:
                    tagName = "h6";
                    break;
                case HtmlTag.Img:
                    tagName = "img";
                    break;
                case HtmlTag.I:
                    tagName = "i";
                    break;
                case HtmlTag.Link:
                    tagName = "link";
                    break;
                case HtmlTag.Li:
                    tagName = "li";
                    break;
                case HtmlTag.Meta:
                    tagName = "meta";
                    break;
                case HtmlTag.Object:
                    tagName = "object";
                    break;
                case HtmlTag.Ol:
                    tagName = "ol";
                    break;
                case HtmlTag.P:
                    tagName = "p";
                    break;
                case HtmlTag.Param:
                    tagName = "param";
                    break;
                case HtmlTag.Span:
                    tagName = "span";
                    break;
                case HtmlTag.Strong:
                    tagName = "strong";
                    break;
                case HtmlTag.Style:
                    tagName = "style";
                    break;
                case HtmlTag.Table:
                    tagName = "table";
                    break;
                case HtmlTag.Tr:
                    tagName = "tr";
                    break;
                case HtmlTag.Th:
                    tagName = "th";
                    break;
                case HtmlTag.Td:
                    tagName = "td";
                    break;
                case HtmlTag.Ul:
                    tagName = "ul";
                    break;
                case HtmlTag.Text:
                    tagName = "unk_text";
                    break;
                case HtmlTag.ScriptObj:
                    tagName = "unk_scriptobj";
                    break;
                case HtmlTag.Unknown1:
                    tagName = "unk_1";
                    break;
                default:
                    throw new ArgumentOutOfRangeException("tag");
            }
            return tagName;
        }
    }
}
