using System.Runtime.CompilerServices;

namespace TheEngine.TheAudio
{
    public class TheAudio : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMuted();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetMute(bool set);

    }
}
