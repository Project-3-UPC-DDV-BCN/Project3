using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheTime
    {
        public static float TimeScale
        {
            get
            {
                return GetTimeScale();
            }
            set
            {
                SetTimeScale(value);
            }
        }

        public static float DeltaTime
        {
            get
            {
                return GetDeltaTime();
            }
        }

        public static float TimeSinceStart
        {
            get
            {
                return GetTimeSinceStart();
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetTimeScale();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float SetTimeScale(float scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetDeltaTime();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetTimeSinceStart();
    }
}
