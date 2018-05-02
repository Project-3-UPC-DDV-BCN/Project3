using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheCamera
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int SizeX();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int SizeY();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern TheVector3 WorldPosToCameraPos(TheVector3 world_pos);
    }
}
