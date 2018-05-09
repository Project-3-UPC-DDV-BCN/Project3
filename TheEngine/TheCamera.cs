using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheCamera : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int SizeX();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int SizeY();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern TheVector3 WorldPosToCameraPos(TheVector3 world_pos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool IsObjectInside(TheVector3 world_pos);


    }
}
