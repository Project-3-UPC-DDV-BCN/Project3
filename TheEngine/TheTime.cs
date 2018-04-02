using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheTime
    {
        public static float TimeScale
        {
            get
            {
                return GetScale();
            }
            set
            {
                SetScale(value);
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
        private static extern float GetScale();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float SetScale(float scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetDeltaTime();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float GetTimeSinceStart();
    }
}
