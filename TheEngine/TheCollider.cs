using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheCollider : TheComponent
    {
        public TheVector3 Center
        {
            get
            {
                return GetColliderCenter();
            }
            set
            {
                SetColliderCenter(value);
            }
        }

        public TheVector3 WorldPosition
        {
            get
            {
                return GetColliderWorldPosition();
            }
        }

        public TheVector3 LocalPosition
        {
            get
            {
                return GetColliderLocalPosition();
            }
        }

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
        private extern TheVector3 GetColliderCenter();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetColliderCenter(TheVector3 center);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetColliderWorldPosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetColliderLocalPosition();

        public override string ToString()
        {
            return "Collider";
        }
    }
}
