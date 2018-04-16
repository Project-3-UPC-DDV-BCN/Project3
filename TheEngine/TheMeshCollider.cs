using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheMeshCollider : TheCollider
    {
        public bool Convex
        {
            get
            {
                return GetMeshColliderConvex();
            }
            set
            {
                SetMeshColliderConvex(value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool GetMeshColliderConvex();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetMeshColliderConvex(bool convex);
    }
}
