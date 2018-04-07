using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheLight : TheComponent
    {
        public TheVector3 Color
        {
            get
            {
                return GetColor();
            }
            set
            {
                SetColor(value);
            }
        }

        /// <summary>
        ///   <para>returns light's RGB color as a TheVector3</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern TheVector3 GetColor();

        /// <summary>
        ///   <para>in RGB, from 0.0 to 255.0, sets light color</para>
        /// </summary>
        /// <param name="color">Color value</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetColor(TheVector3 color);

        /// <summary>
        ///   <para>Activates/Deactivates the LightComponent</para>
        /// </summary>
        /// <param name="value">Active value</param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern void SetActive(bool value);

    }
}
