using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetComponentActive(bool is_active);
    }
}
