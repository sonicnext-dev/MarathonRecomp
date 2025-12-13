///---------------------------------------------------------------------------
//
// (c) Copyright Microsoft Corporation.
// This source is subject to the Microsoft Limited Permissive License.
// See http://www.microsoft.com/resources/sharedsource/licensingbasics/limitedpermissivelicense.mspx
// All other rights reserved.
//
// This file is part of the 3D Tools for Windows Presentation Foundation
// project.  For more information, see:
// 
// http://CodePlex.com/Wiki/View.aspx?ProjectName=3DTools
//
//---------------------------------------------------------------------------

using System;
using System.Collections;
using System.Collections.Generic;
using System.Windows.Media.Media3D;

namespace RageLib.Models.Model3DViewer
{
    /// <summary>
    ///     Matrix3DStack is a stack of Matrix3Ds.
    /// </summary>
    public class Matrix3DStack : IEnumerable<Matrix3D>, ICollection
    {
        public Matrix3D Peek()
        {
            return _storage[_storage.Count - 1];
        }

        public void Push(Matrix3D item)
        {
            _storage.Add(item);
        }

        public void Append(Matrix3D item)
        {
            if (Count > 0)
            {
                Matrix3D top = Peek();
                top.Append(item);
                Push(top);
            }
            else
            {
                Push(item);
            }
        }

        public void Prepend(Matrix3D item)
        {
            if (Count > 0)
            {
                Matrix3D top = Peek();
                top.Prepend(item);
                Push(top);
            }
            else
            {
                Push(item);
            }
        }

        public Matrix3D Pop()
        {
            Matrix3D result = Peek();
            _storage.RemoveAt(_storage.Count - 1);

            return result;
        }

        public int Count
        {
            get { return _storage.Count; }
        }

        void Clear()
        {
            _storage.Clear();
        }

        bool Contains(Matrix3D item)
        {
            return _storage.Contains(item);
        }

        private readonly List<Matrix3D> _storage = new List<Matrix3D>();

        #region ICollection Members

        void ICollection.CopyTo(Array array, int index)
        {
            ((ICollection)_storage).CopyTo(array, index);
        }

        bool ICollection.IsSynchronized
        {
            get { return ((ICollection)_storage).IsSynchronized; }
        }

        object ICollection.SyncRoot
        {
            get { return ((ICollection)_storage).SyncRoot; }
        }

        #endregion

        #region IEnumerable Members

        IEnumerator IEnumerable.GetEnumerator()
        {
            return ((IEnumerable<Matrix3D>)this).GetEnumerator();
        }

        #endregion

        #region IEnumerable<Matrix3D> Members

        IEnumerator<Matrix3D> IEnumerable<Matrix3D>.GetEnumerator()
        {
            for (int i = _storage.Count - 1; i >= 0; i--)
            {
                yield return _storage[i];
            }
        }

        #endregion
    }
}