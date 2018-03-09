using System;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace TheEngine
{
    class TheRectTransform : TheComponent
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

        public delegate void MyHandler();

        public event MyHandler OnClick;
        public event MyHandler OnClickDown;
        public event MyHandler OnClickUp;
        public event MyHandler OnMouseEnter;
        public event MyHandler OnMouseOut;
        public event MyHandler OnMouseOver;

        [MethodImpl(MethodImplOptions.InternalCall)]
        private void CallOnClick() { OnClick(); }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private void CallOnClickDown() { OnClickDown(); }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private void CallOnClickUp() { OnClickUp(); }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private void CallOnMouseEnter() { OnMouseEnter(); }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private void CallOnMouseDown() { OnMouseOut(); }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private void CallOnMouseOver() { OnMouseOver(); }


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
