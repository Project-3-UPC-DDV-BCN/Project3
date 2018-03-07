using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheRandom
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int RandomInt();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float RandomFloat();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float RandomRange(float min, float max);
    }
}
