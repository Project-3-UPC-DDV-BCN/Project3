using System.Runtime.CompilerServices;

namespace TheEngine
{
    /// <summary>
    ///   <para>Controls Application</para>
    /// </summary>
    public class TheApplication
    {
        /// <summary>
        ///   <para>Load scene</para>
        /// </summary>
        /// <param name="scene_name">Name of scene to load</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LoadScene(string scene_name);

        /// <summary>
        ///   <para>Exits application</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Quit();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void DrawMouse(bool draw);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void SetMusicPause(bool set);
    }
}
