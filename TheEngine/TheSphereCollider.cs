using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheSphereCollider : TheCollider
    {

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
        private extern float GetSphereColliderRadius();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetSphereColliderRadius(float size);
    }
}
