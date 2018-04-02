using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheResources
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static TheGameObject LoadPrefab(string name);
    }
}
