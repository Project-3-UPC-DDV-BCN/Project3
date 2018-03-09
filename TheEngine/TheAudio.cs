using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheAudio : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMuted();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetMute(bool set);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetVolume();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetVolume(int volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetPitch();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetPitch(int pitch);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRTPCvalue(string name, float value);
    }
}
