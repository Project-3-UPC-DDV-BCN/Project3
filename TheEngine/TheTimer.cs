using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheTimer
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void Start();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern float ReadTime();
    }
}
