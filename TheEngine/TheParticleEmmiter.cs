using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheParticleEmmiter : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Play();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Stop();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetEmitterSpeed(float v);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetParticleSpeed(float v);  
    }
}
