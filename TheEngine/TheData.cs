using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheData
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AddString(string name, string str);
    }
}