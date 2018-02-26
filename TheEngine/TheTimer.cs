using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheTimer
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void CreateTimer(float time);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float ReadTime();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ResetTime();
    }
}
