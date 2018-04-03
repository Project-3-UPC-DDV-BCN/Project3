using System;
using System.Runtime.CompilerServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TheEngine
{
    public class TheObject
    {
        private IntPtr m_CachedPtr;

        internal IntPtr GetCachedPtr()
        {
            return this.m_CachedPtr;
        }

        public extern TheGameObject Self
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            get;
        }
    }
}
