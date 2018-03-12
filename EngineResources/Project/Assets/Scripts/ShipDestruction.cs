using TheEngine;
using TheEngine.Math;

public class ShipDestruction 
{

	public float explosion_v; 

	TheGameObject[] ship_parts; 
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
			SetPartsDirection(); 
			exploted = true; 
		}
				 	
	}

	void FillPartList()
	{

		if(ship_tag == "TIEFIGHTER") 
		{
			for (int i = 0; i < TheGameObject.Self.GetChildCount(); i++) 
			{
				TheGameObject[] objects_to_explote; 
				objects_to_explote = TheGameObject.Self.GetAllChilds(); 
			}
		}
			

		if(ship_tag == "TIEFIGHTER") 
			ship_parts.push_back(TheGameObject.Find("Sphere"));

		
		if(ship_tag == "TIEFIGHTER") 
			ship_parts.push_back(TheGameObject.Find("Sphere"));

		
		if(ship_tag == "TIEFIGHTER") 
			ship_parts.push_back(TheGameObject.Find("Sphere"));


		//Clean it: we only need to disperse object with a mesh renderer on it 
		for(int i = 0; i < objects_to_explote.size;i++)
		{
			if(objects_to_explote[i].GetComponent<TheMeshRenderer>() == null)
				delete(objects_to_explote[i]); 
		}
	
	}


	void SetPartsDirection()
	{

		TheVector3 direction = transform.ForwardDirection.Normalized; 

		for(int i = 0; i < objects_to_explote.size;i++)
		{
			//Random Angle in XZ from 0 to 360 (guess 22)
			int angle_in_xz = 22; 
			
			//Angle in YX from 0 to 360 (guess 78) 
			int angle_in_yz = 78; 

			TheQuaternion fRot; 
			fRot.Set(22,78,0,1);

			//Set the velocity
			TheRigidBody piece_rb = objects_to_explote.GetComponent<TheRigidBody>();
		
			TheQuaternion global_rot = transform.GlobalRotation; 
			TheQuaternion debugQ = TheQuaternion.Slerp(global_rot, fRot, 0);

			TheVector3 converter = debugQ.ToEulerAngles(); 

			direction.x = debugQ.x;
			direction.y = debugQ.y;
			direction.z = debugQ.z;

			direction = direction.Normalized * explosion_v; 

			piece_rb.SetVelocity(direction);		
		}
	}
}