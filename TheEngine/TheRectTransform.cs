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
                SetRectRotation();
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


        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetRectPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetRectPosition(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetRectRotation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 SetRectRotation();

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
