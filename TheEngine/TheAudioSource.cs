using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheAudioSource : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        static private extern bool Play(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static private extern bool Stop(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        static private extern bool Send(string name);
    }
}
