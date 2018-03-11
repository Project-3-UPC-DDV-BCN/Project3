using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheRigidBody : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetLinearVelocity(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetPosition(float x, float y, float z);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetRotation(float x, float y, float z);
    }
}
