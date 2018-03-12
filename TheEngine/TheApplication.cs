using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheApplication
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LoadScene(string scene_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Quit();
    }
}
