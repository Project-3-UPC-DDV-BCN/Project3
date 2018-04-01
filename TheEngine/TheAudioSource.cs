using System.Runtime.CompilerServices;

namespace TheEngine
{
    /// <summary>
    ///   <para>Component to control GameObjects Audios</para>
    /// </summary>
    public class TheAudioSource : TheComponent
    {
        /// <summary>
        ///   <para>Play Audio event</para>
        /// </summary>
        /// <param name="name">Name of event</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool Play(string name);

        /// <summary>
        ///   <para>Stop Audio event</para>
        /// </summary>
        /// <param name="name">Name of event</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool Stop(string name);

        /// <summary>
        ///   <para>Send Audio event</para>
        /// </summary>
        /// <param name="name">Name of event</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool Send(string name);

        /// <summary>
        ///   <para>Sets RTPC value</para>
        /// </summary>
        /// <param name="name">Name of RTPC</param>
        /// <param name="value">RTPC value</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern bool SetMyRTPCvalue(string name, float value);

        /// <summary>
        ///   <para>Sets group state</para>
        /// </summary>
        /// <param name="group">Group</param>
        /// <param name="state">State</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetState(string group, string state);

        /// <summary>
        ///   <para>Sets volume</para>
        /// </summary>
        /// <param name="value">Volume value</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetVolume(int value);
    }
}
