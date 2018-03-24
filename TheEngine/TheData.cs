using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheData
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AddString(string name, string str);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AddInt(string name, int val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AddFloat(string name, float val);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetString(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetInt(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetFloat(string name);
    }
}