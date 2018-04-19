using System.Runtime.CompilerServices;

namespace TheEngine
{
    /// <summary>
    ///   <para>Used to dynamically spawn GameObjects</para>
    /// </summary>
    public class TheFactory : TheComponent
    {
        /// <summary>
        ///   <para>Starts the factory. Creates the number of specified GameObjects</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void StartFactory();

        /// <summary>
        ///   <para>Clears factory and deleted created gamebjects</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void ClearFactory();

        /// <summary>
        ///   <para>Spawns a GameObject</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheGameObject Spawn();

        /// <summary>
        ///   <para>Position where the next GameObjects will be spawned</para>
        /// </summary>
        /// <param name="position"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetSpawnPosition(TheVector3 position);

        /// <summary>
        ///   <para>Rotation of next GameObjects that will be spawned</para>
        /// </summary>
        /// <param name="rotation"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetSpawnRotation(TheVector3 rotation);

        /// <summary>
        ///   <para>Scale of next GameObjects that will be spawned</para>
        /// </summary>
        /// <param name="scale"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetSpawnScale(TheVector3 scale);
    }
}
