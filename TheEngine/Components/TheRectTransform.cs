using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheRectTransform : TheComponent
    {
        public TheVector3 Position
        {
            get
            {
                return GetRectPosition();
            }
            set
            {
                SetRectPosition(value);
            }
        }

        public TheVector3 Rotation
        {
            get
            {
                return GetRectRotation();
            }
            set
            {
                SetRectRotation(value);
            }
        }

        public TheVector3 Size
        {
            get
            {
                return GetRectSize();
            }
            set
            {
                SetRectSize(value);
            }
        }

        public TheVector3 Anchor
        {
            get
            {
                return GetRectAnchor();
            }
            set
            {
                SetRectAnchor(value);
            }
        }

        public bool OnClick
        {
            get
            {
                return GetOnClick();
            }
        }

        public bool OnClickUp
        {
            get
            {
                return GetOnClickUp();
            }
        }

        public bool OnClickDown
        {
            get
            {
                return GetOnClickDown();
            }
        }

        public bool OnMouseOver
        {
            get
            {
                return GetOnMouseOver();
            }
        }

        public bool OnMouseEnter
        {
            get
            {
                return GetOnMouseEnter();
            }
        }

        public bool OnMouseOut
        {
            get
            {
                return GetOnMouseOut();
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool GetOnClick();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool GetOnClickUp();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool GetOnClickDown();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool GetOnMouseOver();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool GetOnMouseEnter();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool GetOnMouseOut();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetRectPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetRectPosition(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetRectRotation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 SetRectRotation(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetRectSize();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetRectSize(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetRectAnchor();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetRectAnchor(TheVector3 value);
    }
}
