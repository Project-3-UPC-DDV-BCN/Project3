using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheTransform : TheComponent
    {
        public TheVector3 LocalPosition
        {
            get
            {
                return GetPosition(false);
            }
            set
            {
                SetPosition(value);
            }
        }

        public TheVector3 LocalRotation
        {
            get
            {
                return GetRotation(false);
            }
            set
            {
                SetRotation(value);
            }
        }

        public TheVector3 LocalScale
        {
            get
            {
                return GetScale(false);
            }
            set
            {
                SetScale(value);
            }
        }

        public TheVector3 GlobalPosition
        {
            get
            {
                return GetPosition(true);
            }
            set
            {
                SetPosition(value);
            }
        }

        public TheVector3 GlobalRotation
        {
            get
            {
                return GetRotation(true);
            }
            set
            {
                SetRotation(value);
            }
        }

        public TheVector3 GlobalScale
        {
            get
            {
                return GetScale(true);
            }
            set
            {
                SetScale(value);
            }
        }

        public TheVector3 ForwardDirection
        {
            get
            {
                return GetForward();
            }
        }

        public TheVector3 UpDirection
        {
            get
            {
                return GetUp();
            }
        }

        public TheVector3 RightDirection
        {
            get
            {
                return GetRight();
            }
        }

        public TheQuaternion QuatRotation
        {
            get
            {
                return GetQuatRotation();
            }
            set
            {
                SetQuatRotation(value);
            }
        }
     
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void RotateAroundAxis(TheVector3 axis, float angle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetPosition(bool is_global);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetPosition(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetRotation(bool is_global);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetRotation(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetIncrementalRotation(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetScale(bool is_global);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetScale(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void LookAt(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void LookAtY(TheVector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetForward();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetRight();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetUp();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheQuaternion GetQuatRotation();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetQuatRotation(TheQuaternion value);

    }
}
