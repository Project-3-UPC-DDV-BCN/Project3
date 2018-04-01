using System.Runtime.CompilerServices;

namespace TheEngine
{
    /// <summary>
    ///   <para>Application Input system</para>
    /// </summary>
    public class TheInput
    {
        /// <summary>
        ///   <para>Maximum joystick movement</para>
        /// </summary>
        public const int MaxJoystickMove = 32767;

        /// <summary>
        ///   <para>Given key is down</para>
        /// </summary>
        /// <param name="key_name">Key to </param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyDown(string key_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyUp(string key_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsKeyRepeat(string key_name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMouseButtonDown(int mouse_button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMouseButtonUp(int mouse_button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsMouseButtonRepeat(int mouse_button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern TheVector3 GetMousePosition();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseXMotion();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetMouseYMotion();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetControllerButton(int pad, string button);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetControllerJoystickMove(int pad, string axis);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int RumbleController(int pad, float strength, int ms);
    }
}
