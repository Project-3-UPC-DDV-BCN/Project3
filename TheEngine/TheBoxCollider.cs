using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheBoxCollider : TheCollider
    {

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
        private extern TheVector3 GetBoxColliderSize();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetBoxColliderSize(TheVector3 size);
    }
}
