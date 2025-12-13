//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  This material may not be duplicated in whole or in part, except for 
//  personal use, without the express written consent of the author. 
//
//  Email:  ianier@hotmail.com
//
//  Copyright (C) 1999-2003 Ianier Munoz. All Rights Reserved.

using System;
using System.Threading;
using System.Runtime.InteropServices;

namespace WaveLib
{
    internal class WaveOutHelper
    {
        public static void Try(int err)
        {
            if (err != WaveNative.MMSYSERR_NOERROR)
                throw new Exception(err.ToString());
        }
    }

    public delegate void BufferFillEventHandler(IntPtr data, int size);

	internal class WaveOutBuffer : IDisposable
	{
        public WaveOutBuffer NextBuffer;

        private AutoResetEvent m_PlayEvent = new AutoResetEvent(false);
        private IntPtr m_WaveOut;

        private WaveNative.WaveHdr m_Header;
        private byte[] m_HeaderData;
        private GCHandle m_HeaderHandle;
        private GCHandle m_HeaderDataHandle;

        private bool m_Playing;

        internal static void WaveOutProc(IntPtr hdrvr, int uMsg, int dwUser, ref WaveNative.WaveHdr wavhdr, int dwParam2)
        {
            if (uMsg == WaveNative.MM_WOM_DONE)
            {
                try
                {
                    GCHandle h = (GCHandle)wavhdr.dwUser;
                    WaveOutBuffer buf = (WaveOutBuffer)h.Target;
                    buf.OnCompleted();
                }
                catch
                {
                }
            }
        }

        public WaveOutBuffer(IntPtr waveOutHandle, int size)
		{
            m_WaveOut = waveOutHandle;

            m_HeaderHandle = GCHandle.Alloc(m_Header, GCHandleType.Pinned);
            m_Header.dwUser = (IntPtr)GCHandle.Alloc(this);
            m_HeaderData = new byte[size];
            m_HeaderDataHandle = GCHandle.Alloc(m_HeaderData, GCHandleType.Pinned);
            m_Header.lpData = m_HeaderDataHandle.AddrOfPinnedObject();
            m_Header.dwBufferLength = size;
            WaveOutHelper.Try(WaveNative.waveOutPrepareHeader(m_WaveOut, ref m_Header, Marshal.SizeOf(m_Header)));
		}
        ~WaveOutBuffer()
        {
            Dispose();
        }
        public void Dispose()
        {
            if (m_Header.lpData != IntPtr.Zero)
            {
                WaveNative.waveOutUnprepareHeader(m_WaveOut, ref m_Header, Marshal.SizeOf(m_Header));
                m_HeaderHandle.Free();
                m_Header.lpData = IntPtr.Zero;
            }
            m_PlayEvent.Close();
            if (m_HeaderDataHandle.IsAllocated)
                m_HeaderDataHandle.Free();
            GC.SuppressFinalize(this);
        }

        public int Size
        {
            get { return m_Header.dwBufferLength; }
        }

        public IntPtr Data
        {
            get { return m_Header.lpData; }
        }

        public bool Play()
        {
            lock(this)
            {
                m_PlayEvent.Reset();
                m_Playing = WaveNative.waveOutWrite(m_WaveOut, ref m_Header, Marshal.SizeOf(m_Header)) == WaveNative.MMSYSERR_NOERROR;
                return m_Playing;
            }
        }
        public void WaitFor()
        {
            if (m_Playing)
            {
                m_Playing = m_PlayEvent.WaitOne();
            }
            else
            {
                Thread.Sleep(0);
            }
        }
        public void OnCompleted()
        {
            m_PlayEvent.Set();
            m_Playing = false;
        }
    }

    public class WaveOutPlayer : IDisposable
    {
        private IntPtr m_WaveOut;
        private WaveOutBuffer m_Buffers; // linked list
        private WaveOutBuffer m_CurrentBuffer;
        private Thread m_Thread;
        private BufferFillEventHandler m_FillProc;
		private bool m_Finished;
		private byte m_zero;

        private WaveNative.WaveDelegate m_BufferProc = new WaveNative.WaveDelegate(WaveOutBuffer.WaveOutProc);

        public static int DeviceCount
        {
            get { return WaveNative.waveOutGetNumDevs(); }
        }

        public WaveOutPlayer(int device, WaveFormat format, int bufferSize, int bufferCount, BufferFillEventHandler fillProc)
        {
			m_zero = format.wBitsPerSample == 8 ? (byte)128 : (byte)0;
            m_FillProc = fillProc;
            WaveOutHelper.Try(WaveNative.waveOutOpen(out m_WaveOut, device, format, m_BufferProc, 0, WaveNative.CALLBACK_FUNCTION));
            AllocateBuffers(bufferSize, bufferCount);
            m_Thread = new Thread(new ThreadStart(ThreadProc));
            m_Thread.Start();
        }
        ~WaveOutPlayer()
        {
            Dispose();
        }
        public void Dispose()
        {
            if (m_Thread != null)
				try
				{
					m_Finished = true;
					if (m_WaveOut != IntPtr.Zero)
						WaveNative.waveOutReset(m_WaveOut);
					m_Thread.Join();
					m_FillProc = null;
					FreeBuffers();
					if (m_WaveOut != IntPtr.Zero)
						WaveNative.waveOutClose(m_WaveOut);
				}
				finally
				{
					m_Thread = null;
					m_WaveOut = IntPtr.Zero;
				}
            //GC.SuppressFinalize(this);
        }
        private void ThreadProc()
        {
            while (!m_Finished)
            {
                Advance();
				if (m_FillProc != null && !m_Finished)
					m_FillProc(m_CurrentBuffer.Data, m_CurrentBuffer.Size);
				else
				{
					// zero out buffer
					byte v = m_zero;
					byte[] b = new byte[m_CurrentBuffer.Size];
					for (int i = 0; i < b.Length; i++)
						b[i] = v;
					Marshal.Copy(b, 0, m_CurrentBuffer.Data, b.Length);

				}
                m_CurrentBuffer.Play();
            }
			WaitForAllBuffers();
		}
        private void AllocateBuffers(int bufferSize, int bufferCount)
        {
            FreeBuffers();
            if (bufferCount > 0)
            {
                m_Buffers = new WaveOutBuffer(m_WaveOut, bufferSize);
                WaveOutBuffer Prev = m_Buffers;
                try
                {
                    for (int i = 1; i < bufferCount; i++)
                    {
                        WaveOutBuffer Buf = new WaveOutBuffer(m_WaveOut, bufferSize);
                        Prev.NextBuffer = Buf;
                        Prev = Buf;
                    }
                }
                finally
                {
                    Prev.NextBuffer = m_Buffers;
                }
            }
        }
        private void FreeBuffers()
        {
            m_CurrentBuffer = null;
            if (m_Buffers != null)
            {
                WaveOutBuffer First = m_Buffers;
                m_Buffers = null;

                WaveOutBuffer Current = First;
                do
                {
                    WaveOutBuffer Next = Current.NextBuffer;
                    Current.Dispose();
                    Current = Next;
                } while(Current != First);
            }
        }
        private void Advance()
        {
            m_CurrentBuffer = m_CurrentBuffer == null ? m_Buffers : m_CurrentBuffer.NextBuffer;
            m_CurrentBuffer.WaitFor();
        }
        private void WaitForAllBuffers()
        {
            WaveOutBuffer Buf = m_Buffers;
            while (Buf.NextBuffer != m_Buffers)
            {
                Buf.WaitFor();
                Buf = Buf.NextBuffer;
            }
        }
    }
}
