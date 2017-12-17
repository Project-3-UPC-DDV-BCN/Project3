﻿using System.Runtime.CompilerServices;

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

        public TheVector3 ToEulerAngles()
        {
            TheVector3 ret = new TheVector3
            {
                x = Math.Math.Atan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z),
                y = Math.Math.Asin(-2 * (x * z - w * y)),
                z = Math.Math.Atan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z)
            };
            return ret;
        }

        public static TheQuaternion FromEulerAngles(TheVector3 angles)
        {
            float cos_z_2 = Math.Math.Cos(0.5f * angles.z);
            float cos_y_2 = Math.Math.Cos(0.5f * angles.y);
            float cos_x_2 = Math.Math.Cos(0.5f * angles.x);

            float sin_z_2 = Math.Math.Sin(0.5f * angles.z);
            float sin_y_2 = Math.Math.Sin(0.5f * angles.y);
            float sin_x_2 = Math.Math.Sin(0.5f * angles.x);

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
            return Math.Math.Acos(Math.Math.Min(Math.Math.Abs(dot), 1f)) * 2f * Math.Math.RadToDeg;
        }

        public static TheQuaternion Lerp(TheQuaternion a, TheQuaternion b, float value)
        {

            return (a * (1 - value) + b * value).Normalized;
        }

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
                float angle = Math.Math.Acos(dot);
                return (a * Math.Math.Sin(angle * (1 - value)) + result * Math.Math.Sin(angle * value)) / Math.Math.Sin(angle);
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
                float value = Math.Math.Min(1f, step / angle);
                result = Slerp(from, to, value);
            }
            return result;
        }

        public static TheQuaternion FromAngleAxis(float angle, TheVector3 axis)
        {
            TheVector3 tmp = axis * Math.Math.Sin(angle / 2);
            TheQuaternion ret = new TheQuaternion
            {
                x = tmp.x,
                y = tmp.y,
                z = tmp.z,
                w = Math.Math.Cos(angle / 2)
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
            angle = Math.Math.Acos(a.w);

            float sin_theta_inv = 1.0f / Math.Math.Sin(angle);

            axis = new TheVector3
            {
                x = a.x * sin_theta_inv,
                y = a.y * sin_theta_inv,
                z = a.z * sin_theta_inv
            };

            angle *= 2;
        }

        public static TheQuaternion LookRotation(TheVector3 direction)
        {
            TheVector3 up = TheVector3.Up;
            return lookRotation(direction, up);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern TheQuaternion lookRotation(TheVector3 direction, TheVector3 up);

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
            return Math.Math.Sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
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

        public static TheQuaternion operator *(TheQuaternion a, TheQuaternion b)
        {
            return new TheQuaternion(
                a.x * b.w + a.y * b.z - a.z * b.y + a.w * b.x,
                a.x * b.z + a.y * b.w + a.z * b.x + a.w * b.y,
                a.x * b.y - a.y * b.x + a.z * b.w + a.w * b.z,
                a.x * b.x - a.y * b.y - a.z * b.z + a.w * b.w
            );
        }

        public static TheQuaternion operator *(TheQuaternion a, float value)
        {
            return new TheQuaternion(a.x * value, a.y * value, a.z * value, a.w * value);
        }

        public static TheVector3 operator *(TheQuaternion rotation, TheVector3 point)
        {
            //float num = rotation.x * 2f;
            //float num2 = rotation.y * 2f;
            //float num3 = rotation.z * 2f;
            //float num4 = rotation.x * num;
            //float num5 = rotation.y * num2;
            //float num6 = rotation.z * num3;
            //float num7 = rotation.x * num2;
            //float num8 = rotation.x * num3;
            //float num9 = rotation.y * num3;
            //float num10 = rotation.w * num;
            //float num11 = rotation.w * num2;
            //float num12 = rotation.w * num3;
            //TheVector3 result = new TheVector3();
            //result.x = (1f - (num5 + num6)) * point.x + (num7 - num12) * point.y + (num8 + num11) * point.z;
            //result.y = (num7 + num12) * point.x + (1f - (num4 + num6)) * point.y + (num9 - num10) * point.z;
            //result.z = (num8 - num11) * point.x + (num9 + num10) * point.y + (1f - (num4 + num5)) * point.z;
            //return result;
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
            return string.Format("({0:F1}, {1:F1}, {2:F1}, {3:F1})", new object[] { x, y, z, w});
        }
    }
}
