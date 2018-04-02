using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheProgressBar : TheComponent
    {
        /// <summary>
        ///   <para>Get the percentatge progress</para>
        /// </summary>
        public float PercentageProgress
        {
            get
            {
                return GetPercentageProgress();
            }
            set
            {
                SetPercentageProgress(value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern float GetPercentageProgress();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern void SetPercentageProgress(float value);
    }
}
