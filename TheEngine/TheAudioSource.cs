using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheAudioSource : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool Play(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool Stop(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool Send(string name);
    }
}
