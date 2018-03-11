using TheEngine;
using TheEngine.Math;
using TheEngine.TheConsole;

public class Ai_Starship_Dflt {

	public float maxSpd = 50.0f;
	public float minSpd = 5.0f;
	public float currSpd = 0.0f;

	public float acceleration = 1.0f;

	public int movState = 1;

	public float Mnv = 5.0f;
	public float capAngle = 5.0f;
	public TheGameObject target = null;

	TheTransform transform;

	public float xangle = 0;
	public float yangle = 0;
	public float zangle = 0;
	public float wangle = 0;

	void Start () {
		transform = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		// Propulsion
		switch(movState) {
			case 0: // Idle
				break;
			case 1: // Accelerate
				currSpd += acceleration;
				break;
			case 2: // Decelerate
				currSpd -= acceleration;
				break;
		}

		if(currSpd < minSpd) {
			currSpd = minSpd;
		}
		if(currSpd > maxSpd) {
			currSpd = maxSpd;
		}

		transform.LocalPosition += transform.ForwardDirection * currSpd * TheTime.DeltaTime;

		// AlignToTarget

		// ROTATION PART HERE \/\/\/ -----------		

		if(target != null) { // YOU NEED A TARGET (USE 'New GameObject(4)')
			TheTransform tTrans = target.GetComponent<TheTransform>();
			//TheVector3 fdir = (tTrans.GlobalPosition - transform.GlobalPosition).Normalized;
			//TheVector3 fwd = transform.ForwardDirection.Normalized;
			//TheVector3 hfwd = TheVector3.Zero;
			//hfwd.x = fwd.x;
			//hfwd.z = fwd.z;
			//TheVector3 vfwd = TheVector3.Zero;
			//vfwd.y = fwd.y;
			//vfwd.z = fwd.z;
			//TheVector3 hfdir = TheVector3.Zero;
			//hfdir.x = fdir.x;
			//hfdir.z = fdir.z;
			//TheVector3 vfdir = TheVector3.Zero;
			//vfdir.y = fdir.y;
			//vfdir.z = fdir.z;
			//float vAngle = TheVector3.AngleBetween(hfwd, hfdir);
			//float vAngle = AngleBetween3D(vfwd, vfdir);
			//float hAngle = TheVector3.AngleBetween(vfwd, vfdir);
			//float hAngle = AngleBetween3D(hfwd, hfdir);
			//xangle = hAngle;
			//yangle = vAngle;
			//TheVector3 comparingV = transform.GlobalPosition + fwd;
			//TheVector3 enemyV = transform.GlobalPosition + fdir;
			//if(vAngle > capAngle) {
			//	if(comparingV.x > enemyV.x)
			//		transform.LocalRotation = new TheVector3(transform.LocalRotation.x + Mnv * TheTime.DeltaTime, transform.GlobalRotation.y, transform.LocalRotation.z);
			//	else {
			//		transform.LocalRotation = new TheVector3(transform.LocalRotation.x - Mnv * TheTime.DeltaTime, transform.GlobalRotation.y, transform.LocalRotation.z);
			//	}
			//}			
			//if(hAngle > capAngle) {
			//	if(comparingV.y > enemyV.y)
			//		transform.LocalRotation = new TheVector3(transform.LocalRotation.x, transform.LocalRotation.y + Mnv * TheTime.DeltaTime, transform.LocalRotation.z);
			//	else
			//		transform.LocalRotation = new TheVector3(transform.LocalRotation.x, transform.LocalRotation.y - Mnv * TheTime.DeltaTime, transform.LocalRotation.z);
			//}

			// Quaternion approach

			TheQuaternion qGlobalRot = transform.LocalRotation.ToQuaternion();
			TheQuaternion fRot = QuaternionLookRotation((tTrans.GlobalPosition - transform.GlobalPosition) * TheMath.DegToRad, TheVector3.Up); // THIS IS THE FINAL ROTATION QUAT
			TheQuaternion debugQ = TheQuaternion.Slerp(qGlobalRot, fRot, Mnv * TheTime.DeltaTime);
			TheConsole.Log(TheQuaternion.Slerp(qGlobalRot, fRot, Mnv * TheTime.DeltaTime));
			TheConsole.Log(TheQuaternion.Slerp2(qGlobalRot, fRot, Mnv * TheTime.DeltaTime));
			// THIS IS THE FINAL ROTATIO EULER
			TheVector3 auxConverter = debugQ.ToEulerAngles();
			//auxConverter.x = auxConverter.x * TheMath.RadToDeg;
			//auxConverter.y = auxConverter.y * TheMath.RadToDeg;
			//auxConverter.z = auxConverter.z * TheMath.RadToDeg;
			transform.GlobalRotation = auxConverter * TheMath.RadToDeg;
			//transform.GlobalRotation = TheQuaternion.RotateTowards(qGlobalRot, fRot, Mnv * TheTime.DeltaTime).ToEulerAngles();
			xangle = debugQ.x;
			yangle = debugQ.y;
			zangle = debugQ.z;
			wangle = debugQ.w;
			// Take02
			//transform.LookAt(transform.GlobalPosition + auxConverter * 10.0f);
		}
	
		// GetTarget
		if(target == null) {
			
		}
	}

	/*TheVector3 EulerAnglesFromDir(TheVector3 dir) {
		TheVector3 ret = TheVector3.Zero;		

		ret.x = dir.x - TheVector3.DotProduct(dir.Normalized, TheVector3.Right);
		ret.y = dir.y - TheVector3.DotProduct(dir.Normalized, TheVector3.Up);
		ret.z = dir.z - TheVector3.DotProduct(dir.Normalized, TheVector3.Forward);

		return ret;
	}
	
	TheQuaternion CalculateRotationFromVec(TheVector3 v1, TheVector3 v2) {
		float phi = TheMath.Acos(TheVector3.DotProduct(v1, v2) / TheVector3.Magnitude(v1) * TheVector3.Magnitude(v2));
		float s = 0.5f * TheMath.Sin(phi) / TheMath.Cos(phi/2.0f);
		TheQuaternion ret = TheQuaternion.FromEulerAngles(TheVector3.Zero);
		ret.x = TheVector3.Normalize(TheVector3.CrossProduct(v1, v2)).x * s;
		ret.y = TheVector3.Normalize(TheVector3.CrossProduct(v1, v2)).y * s;
		ret.z = TheVector3.Normalize(TheVector3.CrossProduct(v1, v2)).z * s;
		ret.w = TheMath.Cos(phi/2.0f);
		return ret;
	}

	float AngleBetween2DVecs(TheVector3 v1, TheVector3 v2) {
		return TheMath.Atan2(v2.y - v1.y, v1.x - v2.y);
	}

	float AngleBetween3D(TheVector3 v1, TheVector3 v2) {
		return TheMath.Acos(Dot(v1, v2) / (TheVector3.Magnitude(v1) * TheVector3.Magnitude(v2))) * TheMath.RadToDeg;
	}*/

	public TheQuaternion QuaternionLookRotation(TheVector3 forward, TheVector3 up)
 	{
     	TheVector3.Normalize(forward);
 
     	TheVector3 vector = forward.Normalized;
     	TheVector3 vector2 = TheVector3.CrossProduct(up, vector).Normalized;
     	TheVector3 vector3 = TheVector3.CrossProduct(vector, vector2);
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
     	TheQuaternion quaternion = TheQuaternion.FromEulerAngles(TheVector3.Zero);
     	if (num8 > 0f)
     	{
         	float num = TheMath.Sqrt(num8 + 1f);
         	quaternion.w = num * 0.5f;
         	num = 0.5f / num;
         	quaternion.x = (m12 - m21) * num;
         	quaternion.y = (m20 - m02) * num;
         	quaternion.z = (m01 - m10) * num;
         	return quaternion;
     	}
     	if ((m00 >= m11) && (m00 >= m22))
     	{
         	float num7 = TheMath.Sqrt(((1f + m00) - m11) - m22);
         	float num4 = 0.5f / num7;
         	quaternion.x = 0.5f * num7;
         	quaternion.y = (m01 + m10) * num4;
         	quaternion.z = (m02 + m20) * num4;
        	quaternion.w = (m12 - m21) * num4;
      	 	return quaternion;
     	}
     	if (m11 > m22)
     	{
         	float num6 = TheMath.Sqrt(((1f + m11) - m00) - m22);
         	float num3 = 0.5f / num6;
         	quaternion.x = (m10+ m01) * num3;
         	quaternion.y = 0.5f * num6;
         	quaternion.z = (m21 + m12) * num3;
         	quaternion.w = (m20 - m02) * num3;
      	 	return quaternion; 
     	}
     	float num5 = TheMath.Sqrt(((1f + m22) - m00) - m11);
     	float num2 = 0.5f / num5;
     	quaternion.x = (m20 + m02) * num2;
     	quaternion.y = (m21 + m12) * num2;
     	quaternion.z = 0.5f * num5;
     	quaternion.w = (m01 - m10) * num2;
     	return quaternion;

		// Take02
		//forward = forward.Normalized;
		//up = up.Normalized;
		//TheVector3 s = TheVector3.CrossProduct(forward, up);
		//TheVector3 un = TheVector3.CrossProduct(forward, s);
		//float m00 = forward.x;
		//float m01 = forward.y;
		//float m02 = forward.z;
		//float m10 = un.x;
		//float m11 = un.y;
		//float m12 = un.z;
		//float m20 = s.x;
		//float m21 = s.y;
		//float m22 = s.z;
		//float qw = 0;
		//float qx = 0;
		//float qy = 0;
		//float qz = 0;
		//float tr = m00 + m11 + m22;		
		//if (tr > 0) { 
 		//	float S = 0.5f / TheMath.Sqrt(tr + 1.0f);// * 2f; // S=4*qw 
  		//	qw = 0.25f * S;
  		//	qx = (m21 - m12) / S;
  		//	qy = (m02 - m20) / S; 
  		//	qz = (m10 - m01) / S; 
		//} else if ((m00 > m11)&(m00 > m22)) { 
  		//	float S = TheMath.Sqrt(1.0f + m00 - m11 - m22)*2f; // S=4*qx 
  		//	qw = (m21 - m12) / S;
  		//	qx = 0.25f * S;
  		//	qy = (m01 + m10) / S; 
  		//	qz = (m02 + m20) / S; 
		//} else if (m11 > m22) { 
  		//	float S = TheMath.Sqrt(1.0f + m11 - m00 - m22)*2f; // S=4*qy
  		//	qw = (m02 - m20) / S;
  		//	qx = (m01 + m10) / S; 
  		//	qy = 0.25f * S;
  		//	qz = (m12 + m21) / S; 
		//} else { 
  		//	float S = TheMath.Sqrt(1.0f + m22 - m00 - m11) *2f; // S=4*qz
  		//	qw = (m10 - m01) / S;
  		//	qx = (m02 + m20) / S;
  		//	qy = (m12 + m21) / S;
  		//	qz = 0.25f * S;
		//}
		//TheQuaternion q = TheQuaternion.Identity;
		//q.x = qx;
		//q.y = qy;
		//q.z = qz;
		//q.w = qw;
		//return q;
		
 	}
	
	/*float Dot(TheVector3 a, TheVector3 b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	TheQuaternion QFromEulerAngles(TheVector3 v) {
		TheQuaternion q = TheQuaternion.Identity;
		float num9 = v.z * 0.5f;
        float num6 = TheMath.Sin(num9);
        float num5 = TheMath.Cos(num9);
        float num8 = v.y * 0.5f;
        float num4 = TheMath.Sin(num8);
        float num3 = TheMath.Cos(num8);
        float num7 = v.x * 0.5f;
        float num2 = TheMath.Sin(num7);
        float num = TheMath.Cos(num7);
        q.x = ((num * num4) * num5) + ((num2 * num3) * num6);
        q.y = ((num2 * num3) * num5) - ((num * num4) * num6);
        q.z = ((num * num3) * num6) - ((num2 * num4) * num5);
        q.w = ((num * num3) * num5) + ((num2 * num4) * num6);
		return q;
	}

	TheQuaternion SlerpQ(TheQuaternion quaternion1, TheQuaternion quaternion2, float amount) {
		float num2;
        float num3;
        TheQuaternion quaternion = TheQuaternion.Identity;
        float num = amount;
        float num4 = (((quaternion1.x * quaternion2.x) + (quaternion1.y * quaternion2.y)) + (quaternion1.z * quaternion2.z)) + (quaternion1.w * quaternion2.w);
        bool flag = false;
        if (num4 < 0f)
        {
        flag = true;
        num4 = -num4;
        }
        if (num4 > 0.999999f)
        {
            num3 = 1f - num;
        	num2 = flag ? -num : num;
        }
        else
        {
            float num5 = TheMath.Acos(num4);
            float num6 = (1.0f / TheMath.Sin(num5));
            num3 = (TheMath.Sin(((1f - num) * num5))) * num6;
       		num2 = flag ? ((-TheMath.Sin((num * num5))) * num6) : ((TheMath.Sin((num * num5))) * num6);
        }
        quaternion.x = (num3 * quaternion1.x) + (num2 * quaternion2.x);
        quaternion.y = (num3 * quaternion1.y) + (num2 * quaternion2.y);
        quaternion.z = (num3 * quaternion1.z) + (num2 * quaternion2.z);
        quaternion.w = (num3 * quaternion1.w) + (num2 * quaternion2.w);
    	return quaternion;
	}

	// TOO SIMILAR TO SLERPQ ORIGINAL
	TheQuaternion SlerpQ2(TheQuaternion q1, TheQuaternion q2, float t) {
		float to0, to1, to2, to3;
		float omega, cosom, sinom, scale0, scale1;
		cosom = q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
		if(cosom < 0.0f) { 
			cosom = -cosom; 
			to0 = -q2.x;
			to1 = -q2.y;
			to2 = -q2.z;
			to3 = -q2.w;
		} else {
			to0 = q2.x;
			to1 = q2.y;
			to2 = q2.z;
			to3 = q2.w;
		}
		if((1.0f - cosom) > 0.999999f) {
			omega = TheMath.Acos(cosom);
			sinom = TheMath.Sin(omega);
			scale0 = TheMath.Sin(t * omega) / sinom;
			scale1 = TheMath.Sin(t * omega) / sinom;
		}
		else {
			scale0 = 1.0f - t;
			scale1 = t;
		}
		TheQuaternion ret = TheQuaternion.Identity;
		ret.x = scale0 * q1.x + scale1 * to0;
		ret.y = scale0 * q1.y + scale1 * to1;
		ret.z = scale0 * q1.z + scale1 * to2;
		ret.w = scale0 * q1.w + scale1 * to3;
		return ret;
	}

	TheVector3 QToEulerAngles(TheQuaternion q) {
		TheVector3 v = TheVector3.Zero;
		v.x = TheMath.Atan((2f*(q.x * q.y + q.z * q.w)) / (1f - 2f*(q.y * q.y + q.z * q.z)));
		v.y = TheMath.Asin(2f*(q.x * q.z - q.w * q.y));
		v.z = TheMath.Atan((2f*(q.x*q.w + q.y*q.z)) / (1 - 2f*(q.z * q.z + q.w * q.w)));
		return v;
	}*/

}