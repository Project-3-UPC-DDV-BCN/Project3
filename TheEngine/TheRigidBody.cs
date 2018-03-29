using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheRigidBody : TheComponent
    {

        public bool Kinematic
        {
            get
            {
                return IsKinematic(); 
            }

            set
            {
                SetKinematic(value);
            }
        }

        public bool TransformGO
        {
            get
            {
                return IsTransformGO(); 
            }

            set
            {
                SetTransformGO(value); 
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetKinematic(bool kinematic);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetAngularVelocity(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void AddTorque(float x, float y, float z, int force_type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool IsKinematic();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetTransformGO(bool transform);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern bool IsTransformGO();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetLinearVelocity(float x, float y, float z);

        //[MethodImpl(MethodImplOptions.InternalCall)]
        //public extern void DisableCollider(int index);

        //[MethodImpl(MethodImplOptions.InternalCall)]
        //public extern void DisableAllColliders();

        //[MethodImpl(MethodImplOptions.InternalCall)]
        //public extern void EnableCollider(int index);

        //[MethodImpl(MethodImplOptions.InternalCall)]
        //public extern void EnableAllColliders();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetPosition(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRotation(float x, float y, float z);
    }
}
