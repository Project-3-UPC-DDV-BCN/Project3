using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class ThePrefab : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheGameObject GetGameObject();
    }
}
