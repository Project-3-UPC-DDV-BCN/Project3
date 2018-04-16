using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheSphereCollider : TheCollider
    {
        public TheVector3 Center
        {
            get
            {
                return GetSphereColliderCenter();
            }
            set
            {
                SetSphereColliderCenter(value);
            }
        }

        public float Radius
        {
            get
            {
                return GetSphereColliderRadius();
            }
            set
            {
                SetSphereColliderRadius(value);
            }
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetSphereColliderCenter();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetSphereColliderCenter(TheVector3 center);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern float GetSphereColliderRadius();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetSphereColliderRadius(float size);
    }
}
