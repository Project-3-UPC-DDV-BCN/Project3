using System.Runtime.CompilerServices;

namespace TheEngine
{
    class TheRectTransform : TheComponent
    {
        public TheVector3 Position
        {
            get
            {
                return GetPosition();
            }
            set
            {
                SetPosition(value);
            }
        }

        public TheVector3 Rotation
        {
            get
            {
                return GetRotation();
            }
            set
            {
                SetRotation();
            }
        }

        public TheVector3 Size
        {
            get
            {
                return GetSize();
            }
            set
            {
                SetSize(value);
            }
        }

        public TheVector3 Anchor
        {
            get
            {
                return GetAnchor();
            }
            set
            {
                SetAnchor(value);
            }
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetPosition(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetRotation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 SetRotation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetSize();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetSize(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetAnchor();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetAnchor(TheVector3 value);
    }
}
