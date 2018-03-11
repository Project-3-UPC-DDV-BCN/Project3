using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class Application
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LoadScene(string scene_name);
    }
}
