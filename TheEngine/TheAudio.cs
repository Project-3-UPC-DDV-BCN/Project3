using System.Runtime.CompilerServices;

namespace TheEngine
{
    /// <summary>
    ///   <para>Controls the Application Audio</para>
    /// </summary>
    public class TheAudio : TheComponent
    {
        /// <summary>
        ///   <para>Returns if Audio is muted</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMuted();

        /// <summary>
        ///   <para>Sets if Audio is muted</para>
        /// </summary>
        /// <param name="set">Muted value</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetMute(bool set);

        /// <summary>
        ///   <para>Returns Audio volume</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetVolume();

        /// <summary>
        ///   <para>Sets Audio volume</para>
        /// </summary>
        /// <param name="volume">Volume value</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetVolume(int volume);

        /// <summary>
        ///   <para>Returns Audio pitch</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetPitch();

        /// <summary>
        ///   <para>Sets Audio pitch</para>
        /// </summary>
        /// <param name="pitch">Pitch value</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetPitch(int pitch);

        /// <summary>
        ///   <para>Sets RTPC value</para>
        /// </summary>
        /// <param name="name">RTPC name</param>
        /// <param name="value">value to set</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRTPCvalue(string name, float value);
    }
}
