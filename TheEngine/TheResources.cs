using System.Runtime.CompilerServices;

namespace TheEngine
{
    class TheResources
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        extern static TheGameObject LoadPrefab(string name);
    }
}
