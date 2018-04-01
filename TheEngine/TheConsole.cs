using System.Runtime.CompilerServices;

namespace TheEngine.TheConsole
{
    /// <summary>
    ///   <para>Send messages to editor console</para>
    /// </summary>
    public class TheConsole
    {
        /// <summary>
        ///   <para>Logs messages to editor console</para>
        /// </summary>
        /// <param name="message">Message to send</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Log(object message);

        /// <summary>
        ///   <para>Logs warnings messages to editor console</para>
        /// </summary>
        /// <param name="message">Message to send</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Warning(object message);

        /// <summary>
        ///   <para>Logs errors messages to editor console</para>
        /// </summary>
        /// <param name="message">Message to send</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Error(object message);
    }
}
