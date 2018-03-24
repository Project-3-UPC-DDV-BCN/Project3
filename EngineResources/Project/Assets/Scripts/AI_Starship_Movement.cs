using TheEngine;
using TheEngine.TheMath;
using System.Collections.Generic;
public class Ai_Starship_Movement {

	public float maxSpd = 50.0f;
	public float minSpd = 5.0f;
	public float currSpd = 0.0f;

	public float acceleration = 1.0f;

	public int movState = 1;

	public float Mnv = 5.0f;
	public float capAngle = 5.0f;
	public TheGameObject target = null;

	TheTransform transform;

	public float roll_rotate_speed = 100.0f;
	public float pitch_rotate_speed = 100.0f;
	public float yaw_rotate_speed = 100.0f;
	public float max_angle = 10.0f;

    TheGameObject gameobject;

	public float xangle = 0;
	public float yangle = 0;
	public float zangle = 0;
	public float wangle = 0;

	public bool Interpolate = false;

    public bool Separation = true;
    public float repulsion_spd = 5.0f;
    public float range = 10.0f;
    public float break_range = 5.0f;
    TheGameObject[] all_gos;
    TheVector3 repulsion_v = TheVector3.Zero;

    public float min_target_range = 20.0f;

	TheGameObject auxTarget = null;

	public int rand = 0;

    void Start () {
		transform = TheGameObject.Self.GetComponent<TheTransform>();
        gameobject = TheGameObject.Self;
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
		rand = 6;
		TheVector3 toTDir = transform.ForwardDirection;
        // Targeting
        if(target == null)
        {
            TheGameObject[] ships_in_scene = new TheGameObject[500];
			//List<TheGameObject> ships_in_scene = new List<TheGameObject>();
            int nship = 0;
            foreach (TheGameObject go in TheGameObject.GetSceneGameObjects())
            {
                if(gameobject.tag == "Alliance" && go.tag == "Empire" || gameobject.tag == "Empire" && go.tag == "Alliance")
                {
     				//ships_in_scene.Add(go);               
					ships_in_scene[nship++] = go;
                }
            }
			int newS = 0;
			foreach(TheGameObject go in ships_in_scene) {
				if(go == null)
					break;
				newS++;
			}
			TheGameObject[] auxList = new TheGameObject[newS];
			for(int i = 0; i < newS; i++) {
				auxList[i] = ships_in_scene[i];
			}
            if(auxList.Length > 0)
            {
				target = auxList[0];
				foreach(TheGameObject ship in auxList) {
					float distA = TheVector3.Magnitude(transform.GlobalPosition - ship.GetComponent<TheTransform>().GlobalPosition);
					float currDistB  = TheVector3.Magnitude(transform.GlobalPosition - target.GetComponent<TheTransform>().GlobalPosition);
					if(currDistB > distA) {
						//if(target != auxTarget && target != null)
							target = ship;
					}
				}
				//rand = (int)TheRandom.RandomRange(0f, auxList.Length);
				//target = ships_in_scene[0];
                //target = ships_in_scene[rand];
            }
        } 
		else
        {
           /*
		   float disto = TheVector3.Magnitude(transform.GlobalPosition - target.GetComponent<TheTransform>().GlobalPosition);
           if (disto < min_target_range)
           {
				auxTarget = target;
				
                target = null;
				TheGameObject[] ships_in_scene = new TheGameObject[500];
				List<TheGameObject> ships_in_scene = new List<TheGameObject>();
		     	int nship = 0;
            	foreach (TheGameObject go in TheGameObject.GetSceneGameObjects())
            	{
                	if(gameobject.tag == "Alliance" && go.tag == "Empire" || gameobject.tag == "Empire" && go.tag == "Alliance")
                	{
     					ships//_in_scene.Add(go);               
						ships_in_scene[nship++] = go;
                 	}
				}
				int newS =  0;
				foreach(TheGameObject go in ships_in_scene) {
					if (go == null)
						break;
					newS++;
				}
				TheGameObject[] auxList = new TheGameObject[newS];
				for(int i = 0; i < newS; i++) {
					auxList[i] = ships_in_scene[i];
				}
				if(auxList.Length > 0)
				{
					target = auxList[0];
					foreach(TheGameObject ship in auxList) {
						float distA = TheVector3.Magnitude(transform.GlobalPosition - ship.GetComponent<TheTransform>().GlobalPosition);
						float currDistB  = TheVector3.Magnitude(transform.GlobalPosition - target.GetComponent<TheTransform>().GlobalPosition);
						if(currDistB > distA) {
							if(ship != auxTarget && ship != null)
								target = ship;
						}
					}
					rand = (int)TheRandom.RandomRange(0f, auxList.Length);
					target = ships_in_scene[0];
					target = ships_in_scene[rand];
				}
		   }
			*/
		}
		// AlignToTarget

		// ROTATION PART HERE \/\/\/ -----------		

		if(target != null) { // YOU NEED A TARGET (USE 'New GameObject(4)')
			TheTransform tTrans = target.GetComponent<TheTransform>();
			
			// No Rotations Baby
			toTDir = tTrans.GlobalPosition - transform.GlobalPosition;
			
			TheVector3 direction = toTDir;
			float x_ = TheVector3.AngleBetween(transform.ForwardDirection, new TheVector3(tTrans.GlobalPosition.x, 0, tTrans.GlobalPosition.z));
			float y_ = TheVector3.AngleBetween(transform.ForwardDirection, new TheVector3(0, toTDir.y, toTDir.z));
			float z_ = TheVector3.AngleBetween(transform.ForwardDirection, new TheVector3(toTDir.x, toTDir.y, 0));
			
			TheVector3 new_rot = transform.LocalRotation;
			if (x_ > 2 )
			{
				new_rot.y -= yaw_rotate_speed*TheTime.DeltaTime;
			}
			else if (x_ < -2)
			{
				new_rot.y += yaw_rotate_speed*TheTime.DeltaTime;
			}
			/*if (y_ > 2 )
			{
				new_rot.x += pitch_rotate_speed*TheTime.DeltaTime;
			}
			else if (y_ < -2)
			{
				new_rot.x -= pitch_rotate_speed*TheTime.DeltaTime;
			}
			if (z_ > 2 )
			{
				new_rot.y -= yaw_rotate_speed*TheTime.DeltaTime;
			}
			else if (z_ < -2)
			{
				new_rot.y += yaw_rotate_speed*TheTime.DeltaTime;
			}*/
			transform.LocalRotation = new_rot;
			/*
			if (direction.y > 0 && TheVector3.AngleBetween(transform.ForwardDirection, toTDir) > max_angle)
			{
				TheVector3 new_rot = transform.LocalRotation;
				new_rot.x += pitch_rotate_speed*TheTime.DeltaTime;
				transform.LocalRotation = new_rot;
			}
			else if (direction.y < 0 && TheVector3.AngleBetween(transform.ForwardDirection, toTDir) > max_angle)
			{
				TheVector3 new_rot = transform.LocalRotation;
				new_rot.x -= pitch_rotate_speed*TheTime.DeltaTime;
				transform.LocalRotation = new_rot;
			}
			if (direction.z > 0 && TheVector3.AngleBetween(transform.ForwardDirection, toTDir) > max_angle)
			{
				TheVector3 new_rot = transform.LocalRotation;
				new_rot.z += roll_rotate_speed*TheTime.DeltaTime;
				transform.LocalRotation = new_rot;
			}
			else if (direction.z < 0 && TheVector3.AngleBetween(transform.ForwardDirection, toTDir) > max_angle)
			{
				TheVector3 new_rot = transform.LocalRotation;
				new_rot.z -= roll_rotate_speed*TheTime.DeltaTime;
				transform.LocalRotation = new_rot;
			}
			*/
			
			/*
			if (direction.y > 0 && TheVector3.AngleBetween(transform.ForwardDirection, toTDir) > max_angle)
			{
				TheVector3 new_rot = transform.LocalRotation;
				new_rot.x += pitch_rotate_speed*TheTime.DeltaTime;
				transform.LocalRotation = new_rot;
			}
			else if (direction.y < 0 && TheVector3.AngleBetween(transform.ForwardDirection, toTDir) > max_angle)
			{
				TheVector3 new_rot = transform.LocalRotation;
				new_rot.x -= pitch_rotate_speed*TheTime.DeltaTime;
				transform.LocalRotation = new_rot;
			}
			if (direction.z > 0 && TheVector3.AngleBetween(transform.ForwardDirection, toTDir) > max_angle)
			{
				TheVector3 new_rot = transform.LocalRotation;
				new_rot.z += roll_rotate_speed*TheTime.DeltaTime;
				transform.LocalRotation = new_rot;
			}
			else if (direction.z < 0 && TheVector3.AngleBetween(transform.ForwardDirection, toTDir) > max_angle)
			{
				TheVector3 new_rot = transform.LocalRotation;
				new_rot.z -= roll_rotate_speed*TheTime.DeltaTime;
				transform.LocalRotation = new_rot;
			}
			*/
			

            // Separation
            if(Separation)
            {
                repulsion_v = TheVector3.Zero;
                all_gos = TheGameObject.GetSceneGameObjects();
                if (all_gos.Length <= 0)
                {
                    TheVector3 closest_dist = transform.GlobalPosition - all_gos[0].GetComponent<TheTransform>().GlobalPosition;
                    foreach (TheGameObject go in all_gos)
                    {
                        if (go == TheGameObject.Self)
                            continue;
                        TheVector3 dist = transform.GlobalPosition - go.GetComponent<TheTransform>().GlobalPosition;
                        if (TheVector3.Magnitude(dist) < range)
                        {
                            if (TheVector3.Magnitude(closest_dist) > TheVector3.Magnitude(dist))
                                closest_dist = dist;
                            repulsion_v += dist.Normalized;
                        }
                    }
                    float sep_spd = ((range - (TheVector3.Magnitude(closest_dist))) * repulsion_spd) / (range - break_range);
                    TheVector3 reflectV = TheVector3.Reflect(closest_dist, repulsion_v);
                    toTDir += reflectV.Normalized * sep_spd;
                }

			}
		}
		
		transform.LocalPosition += toTDir.Normalized * currSpd * TheTime.DeltaTime;	
		
		// GetTarget
		if(target == null) {
			
		}
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

	TheVector3 VecPerQuat(TheVector3 v, TheQuaternion q) {
		TheVector3 u = TheVector3.Zero;
		u.x = q.x;
		u.y = q.y;
		u.z = q.z;
		float s = q.w;
		TheVector3 vp = 2.0f * TheVector3.DotProduct(u, v) * u
			+ (s * s - TheVector3.DotProduct(u, u)) * v
			+ 2.0f * s * TheVector3.CrossProduct(u, v);
		return vp;
	}

	float AngleBetween3D(TheVector3 v1, TheVector3 v2) {
		return TheMath.Acos(Dot(v1, v2) / (TheVector3.Magnitude(v1) * TheVector3.Magnitude(v2))) * TheMath.RadToDeg;
	}

	public TheQuaternion QuaternionLookRotation(TheVector3 forward, TheVector3 up)
 	{
     	forward = TheVector3.Normalize(forward);
 
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
	
	float Dot(TheVector3 a, TheVector3 b) {
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
        quaternion.x = TheMath.Abs((num3 * quaternion1.x) + (num2 * quaternion2.x));
        quaternion.y = TheMath.Abs((num3 * quaternion1.y) + (num2 * quaternion2.y));
        quaternion.z = TheMath.Abs((num3 * quaternion1.z) + (num2 * quaternion2.z));
        quaternion.w = TheMath.Abs((num3 * quaternion1.w) + (num2 * quaternion2.w));
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
	}

}