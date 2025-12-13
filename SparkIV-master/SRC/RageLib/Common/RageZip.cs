/**********************************************************************\

 RageLib
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
using System.Runtime.InteropServices;

namespace RageLib.Common
{
    public class RageZip
    {
        /*
         * rageZipInflate *rageZipInflateInit(byte *input, int inputLength)
         * bool rageZipInflateProcess(rageZipInflate *rzi, byte *output, int outputLength)
         * void rageZipInflateEnd(rageZipInflate *rzi)
         */

        [DllImport("ragezip.dll", CallingConvention = CallingConvention.StdCall, EntryPoint = "rageZipInflateInit")]
        public static extern IntPtr InflateInit(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1), In] byte[] input,
            int inputLength);

        [DllImport("ragezip.dll", CallingConvention = CallingConvention.StdCall, EntryPoint = "rageZipInflateProcess")]
        public static extern bool InflateProcess(
            IntPtr handle,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2), Out] byte[] output,
            int outputLength);

        [DllImport("ragezip.dll", CallingConvention = CallingConvention.StdCall, EntryPoint = "rageZipInflateEnd")]
        public static extern void InflateEnd(IntPtr handle);
    }
}
