using System.Runtime.CompilerServices;

namespace TheEngine
{
    public class TheVector3
    {
        public TheVector3()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
        }

        public TheVector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public float x;
        public float y;
        public float z;

        private readonly static TheVector3 forward = new TheVector3(0, 0, 1);
        private readonly static TheVector3 backward = new TheVector3(0, 0, -1);
        private readonly static TheVector3 up = new TheVector3(0, 1, 0);
        private readonly static TheVector3 down = new TheVector3(0, -1, 0);
        private readonly static TheVector3 left = new TheVector3(-1, 0, 0);
        private readonly static TheVector3 right = new TheVector3(1, 0, 0);

        public static TheVector3 Zero
        {
            get
            {
                return new TheVector3(); ;
            }
        }

        public static TheVector3 Forward
        {
            get
            {
                return forward;
            }
        }

        public static TheVector3 BackWard
        {
            get
            {
                return backward;
            }
        }

        public static TheVector3 Up
        {
            get
            {
                return up;
            }
        }

        public static TheVector3 Down
        {
            get
            {
                return down;
            }
        }

        public static TheVector3 Left
        {
            get
            {
                return left;
            }
        }

        public static TheVector3 Right
        {
            get
            {
                return right;
            }
        }

        public float Length
        {
            get
            {
                return Magnitude(this);
            }
        }

        public float LengthSquared
        {
            get
            {
                return x * x + y * y + z * z;
            }
        }

        public static float Magnitude(TheVector3 vector)
        {
            return Math.Math.Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
        }

        public TheVector3 Normalized
        {
            get
            {
                return Normalize(this);
            }
        }

        public static TheVector3 Normalize(TheVector3 vector)
        {
            TheVector3 ret;
            float magnitude = Magnitude(vector);
            if (magnitude > 1E-05f)
            {
                ret = vector / magnitude;
            }
            else
            {
                ret = Zero;
            }
            return ret;
        }

        public static TheVector3 Lerp(TheVector3 start, TheVector3 end, float value)
        {
            return start + value * (end - start);
        }

        public static TheVector3 Slerp(TheVector3 start, TheVector3 end, float value)
        {
            float dot = DotProduct(start, end);
            Math.Math.Clamp(dot, -1, 1);
            float theta = Math.Math.Acos(dot) * value;
            TheVector3 relative_vector = end - start * dot;
            return (start * Math.Math.Cos(theta)) + (relative_vector.Normalized * Math.Math.Sin(theta));
        }

        public static TheVector3 NLerp(TheVector3 start, TheVector3 end, float value)
        {
            return Lerp(start, end, value).Normalized;
        }

        public static float DotProduct(TheVector3 a, TheVector3 b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z; 
        }

        public static TheVector3 Project(TheVector3 vector, TheVector3 normal)
        {
            TheVector3 ret;
            float dot = DotProduct(normal, normal);
            if (dot < Math.Math.Epsilon)
            {
                ret = Zero;
            }
            else
            {
                ret = normal * DotProduct(vector, normal) / dot;
            }
            return ret;
        }

        public static float AngleBetween(TheVector3 a, TheVector3 b)
        {
            return Math.Math.Acos(DotProduct(a, b) / DotProduct(a.Normalized, b.Normalized)) * Math.Math.RadToDeg;
        }

        public static float Distance(TheVector3 a, TheVector3 b)
        {
            TheVector3 vector = new TheVector3(a.x - b.x, a.y - b.y, a.z - b.z);
            return Math.Math.Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
        }

        public static TheVector3 MoveTowards(TheVector3 position, TheVector3 target, float step)
        {
            TheVector3 diff = target - position;
            float magnitude = diff.Length;
            TheVector3 ret;
            if (magnitude <= step || magnitude < Math.Math.Epsilon)
            {
                ret = target;
            }
            else
            {
                ret = position + diff / magnitude * step;
            }
            return ret;
        }

        public void Set(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public static TheVector3 CrossProduct(TheVector3 a, TheVector3 b)
        {
            return new TheVector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
        }

        public static TheVector3 Reflect(TheVector3 direction, TheVector3 normal)
        {
            return -2f * DotProduct(normal, direction) * normal + direction;
        }

        public static TheVector3 Scale(TheVector3 a, TheVector3 b)
        {
            return new TheVector3(a.x * b.x, a.y * b.y, a.z * b.z);
        }

        public void Scale(TheVector3 scale)
        {
            x *= scale.x;
            y *= scale.y;
            z *= scale.z;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheQuaternion ToQuaternion();

        public static TheVector3 operator +(TheVector3 a, TheVector3 b)
        {
            return new TheVector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        public static TheVector3 operator -(TheVector3 a, TheVector3 b)
        {
            return new TheVector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        public static TheVector3 operator -(TheVector3 a)
        {
            return new TheVector3(-a.x, -a.y, -a.z);
        }

        public static TheVector3 operator *(TheVector3 a, float value)
        {
            return new TheVector3(a.x * value, a.y * value, a.z * value);
        }

        public static TheVector3 operator *(float value, TheVector3 a)
        {
            return new TheVector3(a.x * value, a.y * value, a.z * value);
        }

        public static TheVector3 operator *(TheVector3 a, TheVector3 b)
        {
            return Scale(a, b);
        }

        public static TheVector3 operator /(TheVector3 a, float value)
        {
            return new TheVector3(a.x / value, a.y / value, a.z / value);
        }

        public static bool operator ==(TheVector3 a, TheVector3 b)
        {
            return a.Equals(b);
        }

        public static bool operator !=(TheVector3 a, TheVector3 b)
        {
            return !(a == b);
        }

        public override string ToString()
        {
            return string.Format("({0:F1}, {1:F1}, {2:F1})", new object[] { x, y, z });
        }

        public override bool Equals(object other)
        {
            bool ret;
            if (other == null || !(other is TheVector3))
            {
                ret = false;
            }
            else
            {
                TheVector3 vector = (TheVector3)other;
                ret = (x == vector.x && y == vector.y && z == vector.z);
            }
            return ret;
        }

        public override int GetHashCode()
        {
            return x.GetHashCode() + y.GetHashCode() + z.GetHashCode();
        }
    }
}
