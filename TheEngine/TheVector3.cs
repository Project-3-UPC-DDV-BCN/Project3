using System.Runtime.CompilerServices;
using TheEngine.TheMath;

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
                return new TheVector3();
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
            return TheMath.TheMath.Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
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
            TheMath.TheMath.Clamp(dot, -1, 1);
            float theta = TheMath.TheMath.Acos(dot) * value;
            TheVector3 relative_vector = end - start * dot;
            return (start * TheMath.TheMath.Cos(theta)) + (relative_vector.Normalized * TheMath.TheMath.Sin(theta));
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
            if (dot < TheMath.TheMath.Epsilon)
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
            return TheMath.TheMath.Acos(DotProduct(a, b) / (Magnitude(a) * Magnitude(b))) * TheMath.TheMath.RadToDeg;
        }

        public static float Distance(TheVector3 a, TheVector3 b)
        {
            TheVector3 vector = new TheVector3(a.x - b.x, a.y - b.y, a.z - b.z);
            return TheMath.TheMath.Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
        }

        public static TheVector3 MoveTowards(TheVector3 position, TheVector3 target, float step)
        {
            TheVector3 diff = target - position;
            float magnitude = diff.Length;
            TheVector3 ret;
            if (magnitude <= step || magnitude < TheMath.TheMath.Epsilon)
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

        public static TheQuaternion RotationToTarget(TheVector3 from, TheVector3 to)
        {
            float a = TheMath.TheMath.Sqrt(from.LengthSquared * to.LengthSquared);
            float b = a + DotProduct(from, to);
            TheVector3 axis;

            if (b < (float)1e-06 * a)
            {
                b = (float)0.0;
                axis = TheMath.TheMath.Abs(from.x) > TheMath.TheMath.Abs(from.z) ? new TheVector3(-from.y, from.x, 0.0f)
                     : new TheVector3((float)0.0, -from.z, from.y);
            }
            else
            {
                axis = CrossProduct(from, to);
            }

            TheQuaternion q = new TheQuaternion(axis.x, axis.y, axis.z, b);
            return q.Normalized;
        }

        public static TheQuaternion QuaternionLookRotation(TheVector3 forward, TheVector3 up)
        {
            Normalize(forward);

            TheVector3 vector = forward.Normalized;
            TheVector3 vector2 = CrossProduct(up, vector).Normalized;
            TheVector3 vector3 = CrossProduct(vector, vector2);
            float m00 = vector2.x;
            float m01 = vector2.y;
            float m02 = vector2.z;
            float m10 = vector3.x;
            float m11 = vector3.y;
            float m12 = vector3.z;
            float m20 = vector.x;
            float m21 = vector.y;
            float m22 = vector.z;


            float num8 = (m00 + m11) + m22;
            TheQuaternion quaternion = TheQuaternion.FromEulerAngles(Zero);
            if (num8 > 0f)
            {
                float num = TheMath.TheMath.Sqrt(num8 + 1f);
                quaternion.w = num * 0.5f;
                num = 0.5f / num;
                quaternion.x = (m12 - m21) * num;
                quaternion.y = (m20 - m02) * num;
                quaternion.z = (m01 - m10) * num;
                return quaternion;
            }
            if ((m00 >= m11) && (m00 >= m22))
            {
                float num7 = TheMath.TheMath.Sqrt(((1f + m00) - m11) - m22);
                float num4 = 0.5f / num7;
                quaternion.x = 0.5f * num7;
                quaternion.y = (m01 + m10) * num4;
                quaternion.z = (m02 + m20) * num4;
                quaternion.w = (m12 - m21) * num4;
                return quaternion;
            }
            if (m11 > m22)
            {
                float num6 = TheMath.TheMath.Sqrt(((1f + m11) - m00) - m22);
                float num3 = 0.5f / num6;
                quaternion.x = (m10 + m01) * num3;
                quaternion.y = 0.5f * num6;
                quaternion.z = (m21 + m12) * num3;
                quaternion.w = (m20 - m02) * num3;
                return quaternion;
            }
            float num5 = TheMath.TheMath.Sqrt(((1f + m22) - m00) - m11);
            float num2 = 0.5f / num5;
            quaternion.x = (m20 + m02) * num2;
            quaternion.y = (m21 + m12) * num2;
            quaternion.z = 0.5f * num5;
            quaternion.w = (m01 - m10) * num2;
            return quaternion;
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
            return string.Format("({0:F4}, {1:F4}, {2:F4})", new object[] { x, y, z });
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
