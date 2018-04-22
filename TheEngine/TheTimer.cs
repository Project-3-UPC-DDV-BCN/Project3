using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheTimer
    {
        private float start_time = 0.0f;
        private bool started = false;

        public void Start()
        {
            start_time = TheTime.TimeSinceStart;
            started = true;
        }

        public void Stop()
        {
            started = false;
            start_time = 0.0f;
        }

        public float ReadTime()
        {
            if (started)
                return TheTime.TimeSinceStart - start_time;
            else
                return 0.0f;
        }
    }
}