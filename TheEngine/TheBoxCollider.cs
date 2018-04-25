using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheBoxCollider : TheCollider
    {
        public TheVector3 Center
        {
            get
            {
                return GetBoxColliderCenter();
            }
            set
            {
                SetBoxColliderCenter(value);
            }
        }

        public TheVector3 Size
        {
            get
            {
                return GetBoxColliderSize();
            }
            set
            {
                SetBoxColliderSize(value);
            }
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetBoxColliderCenter();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetBoxColliderCenter(TheVector3 center);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetBoxColliderSize();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetBoxColliderSize(TheVector3 size);
    }
}
