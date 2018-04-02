using System.Runtime.CompilerServices;
using TheEngine.TheMath;

namespace TheEngine
{
    public class TheQuaternion
    {
        public TheQuaternion()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
            w = 0.0f;
        }

        public TheQuaternion(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public float x;
        public float y;
        public float z;
        public float w;

        public static TheQuaternion Identity
        {
            get
            {
                return new TheQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
            }
        }

        public void Set(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public static float DotProduct(TheQuaternion a, TheQuaternion b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheVector3 ToEulerAngles();

        public static TheQuaternion FromEulerAngles(TheVector3 angles)
        {
            float cos_z_2 = TheMath.TheMath.Cos(0.5f * angles.z);
            float cos_y_2 = TheMath.TheMath.Cos(0.5f * angles.y);
            float cos_x_2 = TheMath.TheMath.Cos(0.5f * angles.x);

            float sin_z_2 = TheMath.TheMath.Sin(0.5f * angles.z);
            float sin_y_2 = TheMath.TheMath.Sin(0.5f * angles.y);
            float sin_x_2 = TheMath.TheMath.Sin(0.5f * angles.x);

            TheQuaternion ret = new TheQuaternion
            {
                x = cos_z_2 * cos_y_2 * sin_x_2 - sin_z_2 * sin_y_2 * cos_x_2,
                y = cos_z_2 * sin_y_2 * cos_x_2 + sin_z_2 * cos_y_2 * sin_x_2,
                z = sin_z_2 * cos_y_2 * cos_x_2 - cos_z_2 * sin_y_2 * sin_x_2,
                w = cos_z_2 * cos_y_2 * cos_x_2 + sin_z_2 * sin_y_2 * sin_x_2
            };

            return ret;
        }

        public static float AngleBetween(TheQuaternion a, TheQuaternion b)
        {
            float dot = DotProduct(a, b);
            return TheMath.TheMath.Acos(TheMath.TheMath.Min(TheMath.TheMath.Abs(dot), 1f)) * 2f * TheMath.TheMath.RadToDeg;
        }

        public static TheQuaternion Lerp(TheQuaternion a, TheQuaternion b, float value)
        {

            return (a * (1 - value) + b * value).Normalized;
        }

        public TheVector3 RotateVector(TheVector3 vector)
        {
            TheVector3 v = new TheVector3(x, y, z);
            TheVector3 v2 = TheVector3.CrossProduct(v, vector) * 2f;
            return vector + v2 * w + TheVector3.CrossProduct(v, v2);
        }

        public TheVector3 UnRotateVector(TheVector3 vector)
        {
            TheVector3 v = new TheVector3(-x, -y, -z);
            TheVector3 v2 = TheVector3.CrossProduct(v, vector) * 2f;
            return vector + v2 * w + TheVector3.CrossProduct(v, v2);
        }

        //public static TheQuaternion Slerp(TheQuaternion a, TheQuaternion b, float value)
        //{
        //    TheQuaternion v0 = a.Normalized;
        //    TheQuaternion v1 = b.Normalized;

        //    // Compute the cosine of the angle between the two vectors.
        //    float dot = DotProduct(v0, v1);

        //    // If the dot product is negative, the quaternions
        //    // have opposite handed-ness and slerp won't take
        //    // the shorter path. Fix by reversing one quaternion.
        //    if (dot < 0.0f)
        //    {
        //        v1 = -v1;
        //        dot = -dot;
        //    }

        //    const double DOT_THRESHOLD = 0.9995;
        //    if (dot > DOT_THRESHOLD)
        //    {
        //        // If the inputs are too close for comfort, linearly interpolate
        //        // and normalize the result.
        //        TheQuaternion result = v0 + value * new TheQuaternion(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z, v1.w - v0.w);
        //        return result.Normalized;
        //    }

        //    TheMath.Clamp(dot, -1, 1);           // Robustness: Stay within domain of acos()
        //    float theta_0 = TheMath.Acos(dot);  // theta_0 = angle between input vectors
        //    float theta = theta_0 * value;    // theta = angle between v0 and result

        //    float s0 = TheMath.Cos(theta) - dot * TheMath.Sin(theta) / TheMath.Sin(theta_0);  // == sin(theta_0 - theta) / sin(theta_0)
        //    float s1 = TheMath.Sin(theta) / TheMath.Sin(theta_0);

        //    return (s0 * v0) + (s1 * v1);
        //}

        public static TheQuaternion Slerp(TheQuaternion a, TheQuaternion b, float value)
        {
            TheQuaternion result;
            float dot = DotProduct(a, b);
            if (dot < 0)
            {
                dot = -dot;
                result = -b;
            }
            else
            {
                result = b;
            }

            if (dot < 0.95f)
            {
                float angle = TheMath.TheMath.Acos(dot);
                return (a * TheMath.TheMath.Sin(angle * (1 - value)) + result * TheMath.TheMath.Sin(angle * value)) / TheMath.TheMath.Sin(angle);
            }
            else
            {
                return Lerp(a, result, value);
            }
        }

        public static TheQuaternion RotateTowards(TheQuaternion from, TheQuaternion to, float step)
        {
            float angle = AngleBetween(from, to);
            TheQuaternion result;
            if (angle == 0f)
            {
                result = to;
            }
            else
            {
                float value = TheMath.TheMath.Min(1f, step / angle);
                result = Slerp(from, to, value);
            }
            return result;
        }

        public static TheQuaternion FromAngleAxis(float angle, TheVector3 axis)
        {
            TheVector3 tmp = axis * TheMath.TheMath.Sin(angle / 2);
            TheQuaternion ret = new TheQuaternion
            {
                x = tmp.x,
                y = tmp.y,
                z = tmp.z,
                w = TheMath.TheMath.Cos(angle / 2)
            };

            return ret;
        }

        public void ToAngleAxis(out float angle, out TheVector3 axis)
        {
            TheVector3 tmp = new TheVector3(x, y, z);
            ToAngleAxis(this, out angle, out axis);
        }

        public static void ToAngleAxis(TheQuaternion a, out float angle, out TheVector3 axis)
        {
            angle = TheMath.TheMath.Acos(a.w);

            float sin_theta_inv = 1.0f / TheMath.TheMath.Sin(angle);

            axis = new TheVector3
            {
                x = a.x * sin_theta_inv,
                y = a.y * sin_theta_inv,
                z = a.z * sin_theta_inv
            };

            angle *= 2;
        }

        public static TheQuaternion FromTwoVectors(TheVector3 u, TheVector3 v)
        {
            float norm_u_norm_v = TheMath.TheMath.Sqrt(TheVector3.DotProduct(u, u) * TheVector3.DotProduct(v, v));
            float real_part = norm_u_norm_v + TheVector3.DotProduct(u, v);
            TheVector3 w;

            if (real_part < 1E-06f * norm_u_norm_v)
            {
                /* If u and v are exactly opposite, rotate 180 degrees
                 * around an arbitrary orthogonal axis. Axis normalisation
                 * can happen later, when we normalise the quaternion. */
                real_part = 0.0f;
                w = TheMath.TheMath.Abs(u.x) > TheMath.TheMath.Abs(u.z) ? new TheVector3(-u.y, u.x, 0.0f)
                                        : new TheVector3(0.0f, -u.z, u.y);
            }
            else
            {
                /* Otherwise, build quaternion the standard way. */
                w = TheVector3.CrossProduct(u, v);
            }

            return Normalize(new TheQuaternion(real_part, w.x, w.y, w.z));
        }

        public static TheQuaternion LookRotation(TheVector3 direction, TheVector3 up)
        {
            if(direction == TheVector3.Zero)
            {
                TheConsole.TheConsole.Log("Zero direction in LookRotation");
                return Identity;
            }

            if(up != direction)
            {
                up = TheVector3.Normalize(up);
                TheVector3 v = direction + up * -TheVector3.DotProduct(up, direction);
                TheQuaternion q = FromTwoVectors(TheVector3.Forward, v);
                return FromTwoVectors(v, direction) * q;
            }
            else
            {
                return FromTwoVectors(TheVector3.Forward, direction);
            }
        }

        //[MethodImpl(MethodImplOptions.InternalCall)]
        //private static extern TheQuaternion lookRotation(TheVector3 direction, TheVector3 up);

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
                return x * x + y * y + z * z + w * w;
            }
        }

        public static float Magnitude(TheQuaternion quaternion)
        {
            return TheMath.TheMath.Sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
        }

        public TheQuaternion Normalized
        {
            get
            {
                return Normalize(this);
            }
        }

        public static TheQuaternion Normalize(TheQuaternion quaternion)
        {
            TheQuaternion ret;
            float magnitude = Magnitude(quaternion);
            if (magnitude > 1E-05f)
            {
                ret = quaternion / magnitude;
            }
            else
            {
                ret = Identity;
            }
            return ret;
        }

        public TheQuaternion Conjugate()
        {
            return new TheQuaternion(-x, -y, -z, w);
        }

        public TheQuaternion Inverse()
        {
            return Conjugate() / LengthSquared;
        }

        public static TheQuaternion operator +(TheQuaternion a, TheQuaternion b)
        {
            return new TheQuaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
        }

        public static TheQuaternion operator -(TheQuaternion a, TheQuaternion b)
        {
            return new TheQuaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
        }

        public static TheQuaternion operator -(TheQuaternion a)
        {
            return new TheQuaternion(-a.x, -a.y, -a.z, -a.w);
        }

        public static TheQuaternion operator *(TheQuaternion lhs, TheQuaternion rhs)
        {
            return new TheQuaternion(
                //a.x * b.w + a.y * b.z - a.z * b.y + a.w * b.x,
                //a.x * b.z + a.y * b.w + a.z * b.x + a.w * b.y,
                //a.x * b.y - a.y * b.x + a.z * b.w + a.w * b.z,
                //a.x * b.x - a.y * b.y - a.z * b.z + a.w * b.w
                lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y, lhs.w * rhs.y + lhs.y * rhs.w + lhs.z * rhs.x - lhs.x * rhs.z, lhs.w * rhs.z + lhs.z * rhs.w + lhs.x * rhs.y - lhs.y * rhs.x, lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
            );
        }

        public static TheQuaternion operator *(TheQuaternion a, float value)
        {
            return new TheQuaternion(a.x * value, a.y * value, a.z * value, a.w * value);
        }

        public static TheQuaternion operator *(float value, TheQuaternion a)
        {
            return new TheQuaternion(a.x * value, a.y * value, a.z * value, a.w * value);
        }

        public static TheVector3 operator *(TheQuaternion rotation, TheVector3 point)
        {
            // Extract the vector part of the quaternion
            TheVector3 u = new TheVector3(rotation.x, rotation.y, rotation.z);

            // Extract the scalar part of the quaternion
            float s = rotation.w;

            // Do the math
            TheVector3 vprime = 2.0f * TheVector3.DotProduct(u, point) * u
                  + (s * s - TheVector3.DotProduct(u, u)) * point
                  + 2.0f * s * TheVector3.CrossProduct(u, point);

            return vprime;
        }

        public static TheQuaternion operator /(TheQuaternion a, float value)
        {
            return new TheQuaternion(a.x / value, a.y / value, a.z / value, a.w / value);
        }

        public static TheQuaternion operator /(TheQuaternion a, TheQuaternion b)
        {
            return a * b.Inverse();
        }

        public static bool operator ==(TheQuaternion a, TheQuaternion b)
        {
            return DotProduct(a, b) > 0.999999f;
        }

        public static bool operator !=(TheQuaternion a, TheQuaternion b)
        {
            return !(a == b);
        }

        public override bool Equals(object other)
        {
            bool ret;
            if (other == null || !(other is TheQuaternion))
            {
                ret = false;
            }
            else
            {
                TheQuaternion quaternion = (TheQuaternion)other;
                ret = (x == quaternion.x && y == quaternion.y && z == quaternion.z && w == quaternion.w);
            }
            return ret;
        }

        public override int GetHashCode()
        {
            return x.GetHashCode() + y.GetHashCode() + z.GetHashCode() + w.GetHashCode();
        }

        public override string ToString()
        {
            return string.Format("({0:F4}, {1:F4}, {2:F4}, {3:F4})", new object[] { x, y, z, w});
        }
    }
}
