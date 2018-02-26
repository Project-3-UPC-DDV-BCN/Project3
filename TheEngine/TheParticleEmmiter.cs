using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheParticleEmmiter : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Play();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Stop();
    }
}
