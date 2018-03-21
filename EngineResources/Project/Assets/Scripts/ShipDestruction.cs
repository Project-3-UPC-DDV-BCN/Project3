using TheEngine;
using TheEngine.TheConsole; 
using System.Collections.Generic; 
using TheEngine.TheMath;

public class ShipDestruction 
{

	public float explosion_v; 

	List<TheGameObject> ship_parts; 

	TheTransform transform; 
	string ship_tag; 

	bool need_boom;
	bool exploted;  

	void Start () 
	{
		transform = TheGameObject.Self.GetComponent<TheTransform>(); 

		need_boom = false; 
		exploted = false; 
	}
	
	void Update ()
	{
		
		if(need_boom == true && exploted == false) 
		{			
			TheConsole.Log("ExploteNow"); 
			FillPartList(); 
			SetPartsDirection(); 		
			exploted = true; 
		}
				 	
	}

	void FillPartList()
	{
		for(int i = 0; i < TheGameObject.Self.GetChildCount(); i++)
		{
			TheGameObject new_part = TheGameObject.Self.GetChild(i); 
			ship_parts.Add(new_part); 
		}
	}


	void SetPartsDirection()
	{

		/*TheVector3 direction = transform.ForwardDirection.Normalized; 

		for(int i = 0; i < ship_parts.Count ;i++)
		{
			//Random Angle in XZ from 0 to 360 (guess 22)
			float angle_in_xz = 22 * TheMath.DegToRad; 
			
			//Angle in YX from 0 to 360 (guess 78) 
			float angle_in_yz = 78 * TheMath.DegToRad; 

			TheQuaternion fRot = null; 
			fRot.Set(angle_in_xz,angle_in_yz,0,1);

			//Set the velocity
			TheRigidBody piece_rb = ship_parts[i].GetComponent<TheRigidBody>();
		
			TheQuaternion global_rot = transform.GlobalRotation.ToQuaternion();
			TheQuaternion debugQ = TheQuaternion.Slerp(global_rot, fRot, 0);

			TheVector3 converter = debugQ.ToEulerAngles(); 

			direction.x = debugQ.x;
			direction.y = debugQ.y;
			direction.z = debugQ.z;

			direction = direction.Normalized * explosion_v; 

			piece_rb.SetLinearVelocity(direction.x, direction.y, direction.z);		
		}*/
	}
}