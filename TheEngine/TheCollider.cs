using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheCollider : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheGameObject GetGameObject();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheRigidBody GetRigidBody();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsTrigger();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetTrigger(bool trigger);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheVector3 ClosestPoint(TheVector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheVector3 GetColliderWorldPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheVector3 GetColliderLocalPosition();

        public override string ToString()
        {
            return "Collider";
        }
    }
}
