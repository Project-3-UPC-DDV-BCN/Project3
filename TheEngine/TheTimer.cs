using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheTimer
    {
        private float start_time = 0.0f;

        public void Start()
        {
            start_time = TheTime.DeltaTime;
        }

        public float ReadTime()
        {
            return TheTime.DeltaTime - start_time;
        }
    }
}
