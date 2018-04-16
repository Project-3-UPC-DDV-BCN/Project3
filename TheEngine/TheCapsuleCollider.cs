using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheCapsuleCollider : TheCollider
    {
        public TheVector3 Center
        {
            get
            {
                return GetCapsuleColliderCenter();
            }
            set
            {
                SetCapsuleColliderCenter(value);
            }
        }

        public float Radius
        {
            get
            {
                return GetCapsuleColliderRadius();
            }
            set
            {
                SetCapsuleColliderRadius(value);
            }
        }

        public float Height
        {
            get
            {
                return GetCapsuleColliderHeight();
            }
            set
            {
                SetCapsuleColliderHeight(value);
            }
        }

        public int Direction
        {
            get
            {
                return GetCapsuleColliderDirection();
            }
            set
            {
                SetCapsuleColliderDirection(value);
            }
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetCapsuleColliderCenter();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetCapsuleColliderCenter(TheVector3 center);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern float GetCapsuleColliderRadius();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetCapsuleColliderRadius(float radius);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern float GetCapsuleColliderHeight();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetCapsuleColliderHeight(float height);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern int GetCapsuleColliderDirection();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetCapsuleColliderDirection(int direction);
    }
}
