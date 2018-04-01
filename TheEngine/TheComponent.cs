using System.Runtime.CompilerServices;

namespace TheEngine
{
    /// <summary>
    ///   <para>Base class for GameObject components</para>
    /// </summary>
    public class TheComponent
    {
        /// <summary>
        ///   <para>Activates/Deactivates component</para>
        /// </summary>
        /// <param name="active">Active value</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetComponentActive(bool active);
    }
}
