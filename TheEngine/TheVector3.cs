using System.Runtime.CompilerServices;
using TheEngine.TheMath;

namespace TheEngine
{
    public class TheVector3
    {
        /// <summary>
        ///   <para>Creates a new vector (0,0,0).</para>
        /// </summary>
        public TheVector3()
        {
            x = 0.0f;
            y = 0.0f;
            z = 0.0f;
        }

        /// <summary>
        ///   <para>Creates a new vector with given x, y, z components.</para>
        /// </summary>
        /// <param name="x">X Component</param>
        /// <param name="y">Y Component</param>
        /// <param name="z">Z Component</param>
        public TheVector3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        /// <summary>
        ///   <para>X component</para>
        /// </summary>
        public float x;

        /// <summary>
        ///   <para>Y component</para>
        /// </summary>
        public float y;

        /// <summary>
        ///   <para>Z component</para>
        /// </summary>
        public float z;

        private readonly static TheVector3 forward = new TheVector3(0, 0, 1);
        private readonly static TheVector3 backward = new TheVector3(0, 0, -1);
        private readonly static TheVector3 up = new TheVector3(0, 1, 0);
        private readonly static TheVector3 down = new TheVector3(0, -1, 0);
        private readonly static TheVector3 left = new TheVector3(-1, 0, 0);
        private readonly static TheVector3 right = new TheVector3(1, 0, 0);

        /// <summary>
		///   <para>Returns vector (0,0,0)</para>
		/// </summary>
        public static TheVector3 Zero
        {
            get
            {
                return new TheVector3();
            }
        }

        /// <summary>
        ///   <para>Returns forward vector (0,0,1)</para>
        /// </summary>
        public static TheVector3 Forward
        {
            get
            {
                return forward;
            }
        }

        /// <summary>
        ///   <para>Returns backward vector (0,0,-1)</para>
        /// </summary>
        public static TheVector3 BackWard
        {
            get
            {
                return backward;
            }
        }

        /// <summary>
        ///   <para>Returns up vector (0,1,0)</para>
        /// </summary>
        public static TheVector3 Up
        {
            get
            {
                return up;
            }
        }

        /// <summary>
        ///   <para>Returns down vector (0,-1,0)</para>
        /// </summary>
        public static TheVector3 Down
        {
            get
            {
                return down;
            }
        }

        /// <summary>
        ///   <para>Returns down vector (-1,0,0)</para>
        /// </summary>
        public static TheVector3 Left
        {
            get
            {
                return left;
            }
        }

        /// <summary>
        ///   <para>Returns down vector (1,0,0)</para>
        /// </summary>
        public static TheVector3 Right
        {
            get
            {
                return right;
            }
        }

        /// <summary>
        ///   <para>Returns the length of the vector</para>
        /// </summary>
        public float Length
        {
            get
            {
                return Magnitude(this);
            }
        }

        /// <summary>
        ///   <para>Returns the squared length of the vector</para>
        /// </summary>
        public float LengthSquared
        {
            get
            {
                return x * x + y * y + z * z;
            }
        }

        /// <summary>
        ///   <para>Returns the magnitude of the given vector</para>
        /// </summary>
        /// <param name="vector">The vector to return its magnitude</param>
        /// <code>
        /// class TestClass 
        /// {
        ///     static int Main() 
        ///     {
        ///         return GetZero();
        ///     }
        /// }
        /// </code>
        public static float Magnitude(TheVector3 vector)
        {
            return TheMath.TheMath.Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
        }

        /// <summary>
        ///   <para>Returns the normalized vector</para>
        /// </summary>
        public TheVector3 Normalized
        {
            get
            {
                return Normalize(this);
            }
        }

        /// <summary>
        ///   <para>Returns the normalized vector of the given vector</para>
        /// </summary>
        /// <param name="vector">The vector to return its normalized vector</param>
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

        /// <summary>
        ///   <para>Linearly interpolates between two vectors</para>
        /// </summary>
        /// <param name="a">From</param>
        /// <param name="b">To</param>
        /// <param name="percent">Lerp value</param>
        public static TheVector3 Lerp(TheVector3 a, TheVector3 b, float percent)
        {
            return a + percent * (b - a);
        }

        /// <summary>
        ///   <para>Spherically interpolates between two vectors</para>
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <param name="percent"></param>
        public static TheVector3 Slerp(TheVector3 a, TheVector3 b, float percent)
        {
            float dot = DotProduct(a, b);
            TheMath.TheMath.Clamp(dot, -1, 1);
            float theta = TheMath.TheMath.Acos(dot) * percent;
            TheVector3 relative_vector = b - a * dot;
            return (a * TheMath.TheMath.Cos(theta)) + (relative_vector.Normalized * TheMath.TheMath.Sin(theta));
        }

        /// <summary>
        ///   <para>Normalized Lerp</para>
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <param name="percent"></param>
        public static TheVector3 NLerp(TheVector3 a, TheVector3 b, float percent)
        {
            return Lerp(a, b, percent).Normalized;
        }

        /// <summary>
        ///   <para>Returns the Dot Product of given vectors</para>
        /// </summary>
        public static float DotProduct(TheVector3 a, TheVector3 b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z; 
        }

        /// <summary>
		///   <para>Projects a vector onto another vector.</para>
		/// </summary>
		/// <param name="vector"></param>
		/// <param name="normal"></param>
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

        /// <summary>
		///   <para>Returns the angle in degrees between a and b.</para>
		/// </summary>
		/// <param name="a">The vector from which the angular difference is measured.</param>
		/// <param name="b">The vector to which the angular difference is measured.</param>
        public static float AngleBetween(TheVector3 a, TheVector3 b)
        {
            return TheMath.TheMath.Acos(DotProduct(a, b) / (Magnitude(a) * Magnitude(b))) * TheMath.TheMath.RadToDeg;
        }

        /// <summary>
		///   <para>Returns the distance between a and b.</para>
		/// </summary>
		/// <param name="a"></param>
		/// <param name="b"></param>
        public static float Distance(TheVector3 a, TheVector3 b)
        {
            TheVector3 vector = new TheVector3(a.x - b.x, a.y - b.y, a.z - b.z);
            return TheMath.TheMath.Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
        }

        /// <summary>
		///   <para>Moves a point current in a straight line towards a target point.</para>
		/// </summary>
		/// <param name="position"></param>
		/// <param name="target"></param>
		/// <param name="step"></param>
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

        /// <summary>
		///   <para>Set x, y and z components of an existing Vector3.</para>
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
        public void Set(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        /// <summary>
		///   <para>Return Cross Product of two vectors.</para>
		/// </summary>
		/// <param name="a"></param>
		/// <param name="b"></param>
        public static TheVector3 CrossProduct(TheVector3 a, TheVector3 b)
        {
            return new TheVector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
        }

        /// <summary>
		///   <para>Reflects a vector off the plane defined by a normal.</para>
		/// </summary>
		/// <param name="direction"></param>
		/// <param name="normal"></param>
        public static TheVector3 Reflect(TheVector3 direction, TheVector3 normal)
        {
            return -2f * DotProduct(normal, direction) * normal + direction;
        }

        /// <summary>
		///   <para>Multiplies two vectors component-wise.</para>
		/// </summary>
		/// <param name="a"></param>
		/// <param name="b"></param>
        public static TheVector3 Scale(TheVector3 a, TheVector3 b)
        {
            return new TheVector3(a.x * b.x, a.y * b.y, a.z * b.z);
        }

        /// <summary>
		///   <para>Multiplies every component of this vector by the same component of scale.</para>
		/// </summary>
		/// <param name="scale"></param>
        public void Scale(TheVector3 scale)
        {
            x *= scale.x;
            y *= scale.y;
            z *= scale.z;
        }

        /// <summary>
		///   <para>Returns Rotation from one vector to another.</para>
		/// </summary>
		/// <param name="from"></param>
        /// <param name="to"></param>
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static TheVector3 RotateTowards(TheVector3 current, TheVector3 target, float angle);
        //{
        //    TheVector3 ortho_vec = TheVector3.CrossProduct(current, target);

        //    TheQuaternion current_quat = current.ToQuaternion();
        //    TheQuaternion target_quat = ortho_vec.ToQuaternion();

        //    target_quat = target_quat.Normalized;
        //    float rad_angle = angle/* * TheMath.TheMath.DegToRad*/;

        //    float cos_result = TheMath.TheMath.Cos(rad_angle / 2);

        //    TheQuaternion q =  target_quat * TheMath.TheMath.Sin(rad_angle / 2);
        //    q.x += cos_result;
        //    q.y += cos_result;
        //    q.z += cos_result;

        //    TheQuaternion q1 = target_quat * TheMath.TheMath.Sin(rad_angle / 2);
        //    q.x -= cos_result;
        //    q.y -= cos_result;
        //    q.z -= cos_result;

        //    TheQuaternion final_quat = q * current_quat * q1;

        //    return final_quat.ToEulerAngles() * TheMath.TheMath.RadToDeg;
        //}

        /// <summary>
        ///   <para>Returns rotation quaternion where the vector is facing</para>
        /// </summary>
        public static TheQuaternion LookRotation(TheVector3 forward, TheVector3 up)
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


        /// <summary>
        ///   <para>Transforms the vector to Quaternion.</para>
        /// </summary>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern TheQuaternion ToQuaternion();

        /// <summary>
        ///   <para>Sums 2 vectors</para>
        /// </summary>
        public static TheVector3 operator +(TheVector3 a, TheVector3 b)
        {
            return new TheVector3(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        /// <summary>
        ///   <para>Subs 2 vectors</para>
        /// </summary>
        public static TheVector3 operator -(TheVector3 a, TheVector3 b)
        {
            return new TheVector3(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        /// <summary>
        ///   <para>Subs current vector with given vector</para>
        /// </summary>
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
