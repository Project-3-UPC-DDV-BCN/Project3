using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheRadar : TheComponent
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void AddEntity(TheGameObject go);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void RemoveEntity(TheGameObject go);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void RemoveAllEntities();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetMarkerToEntity(TheGameObject go, string marker_name);
    }
}

