using System.Runtime.CompilerServices;

namespace TheEngine.Math
{
    public class TheMath
    {
        public const float PI = 3.14159265358979323846f;
        public const float Infinity = float.PositiveInfinity;
        public const float NegativeInfinity = float.NegativeInfinity;
        public const float DegToRad = 0.0174532925199432957f;
        public const float RadToDeg = 57.295779513082320876f;
        public const float Epsilon = 1.401298E-45f;

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsPowerOfTwo(int value);

        public static float Sin(float value)
        {
            return (float)System.Math.Sin(value);
        }

        public static float Cos(float value)
        {
            return (float)System.Math.Cos(value);
        }

        public static float Tan(float value)
        {
            return (float)System.Math.Tan(value);
        }

        public static float Asin(float value)
        {
            return (float)System.Math.Asin(value);
        }

        public static float Acos(float value)
        {
            return (float)System.Math.Acos(value);
        }

        public static float Atan(float value)
        {
            return (float)System.Math.Atan(value);
        }

        public static float Atan2(float value_1, float value_2)
        {
            return (float)System.Math.Atan2(value_1, value_2);
        }

        public static float Sqrt(float value)
        {
            return (float)System.Math.Sqrt(value);
        }

        public static float Abs(float value)
        {
            return System.Math.Abs(value);
        }

        public static int Abs(int value)
        {
            return System.Math.Abs(value);
        }

        public static float Min(float value_1, float value_2)
        {
            return System.Math.Min(value_1, value_2);
        }

        public static float Max(float value_1, float value_2)
        {
            return System.Math.Max(value_1, value_2);
        }

        public static float Pow(float value, float power)
        {
            return (float)System.Math.Pow(value, power);
        }

        public static float Exp(float value)
        {
            return (float)System.Math.Exp(value);
        }

        public static float Log(float value, float new_base)
        {
            return (float)System.Math.Log(value, new_base);
        }

        public static float Ceiling(float value)
        {
            return (float)System.Math.Ceiling(value);
        }

        public static float Floor(float value)
        {
            return (float)System.Math.Floor(value);
        }

        public static float Round(float value)
        {
            return (float)System.Math.Round(value);
        }

        public static float Clamp(float value, float max_value, float min_value)
        {
            if (value < min_value) value = min_value;
            else if (value > max_value) value = max_value;
            return value;
        }

        public static float Lerp(float start, float end, float value)
        {
            return start + value * (end - start);
        }
    }
}
