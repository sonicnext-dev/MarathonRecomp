using System.IO;
using RageLib.Common;
using RageLib.Common.Resources;

namespace RageLib.Models.Resource.Shaders
{
    internal class ShaderFx : Shader, IFileAccess
    {
        private uint Unknown14 { get; set; }
        private uint Unknown15 { get; set; }
        private uint Unknown16 { get; set; }
        private uint Unknown17 { get; set; }

        public string ShaderName { get; private set; }
        public string ShaderSPS { get; private set; }

        #region Implementation of IFileAccess

        public new void Read(BinaryReader br)
        {
            base.Read(br);

            var shaderNamePtr = ResourceUtil.ReadOffset(br);
            var shaderSpsPtr = ResourceUtil.ReadOffset(br);

            Unknown14 = br.ReadUInt32();
            Unknown15 = br.ReadUInt32();
            Unknown16 = br.ReadUInt32();
            Unknown17 = br.ReadUInt32();

            // Data:

            br.BaseStream.Seek(shaderNamePtr, SeekOrigin.Begin);
            ShaderName = ResourceUtil.ReadNullTerminatedString(br);

            br.BaseStream.Seek(shaderSpsPtr, SeekOrigin.Begin);
            ShaderSPS = ResourceUtil.ReadNullTerminatedString(br);
        }

        public new void Write(BinaryWriter bw)
        {
            throw new System.NotImplementedException();
        }

        #endregion
    }
}