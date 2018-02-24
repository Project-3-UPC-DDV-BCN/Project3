using System.Runtime.CompilerServices;

namespace TheEngine
{
    class TheProgressBar : TheComponent
    {
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
