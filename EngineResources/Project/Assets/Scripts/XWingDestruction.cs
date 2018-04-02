using TheEngine;
using TheEngine.TheConsole; 
using System.Collections.Generic; 

public class XWingDestruction {
	
	public float explosion_v; 
	public float time_to_destroy; 

	private TheTimer destroy_timer = new TheTimer();
    private TheScript properties; 

	List<TheGameObject> ship_parts; 

	TheTransform transform; 
	string ship_tag; 

	bool need_boom;
	bool exploted;
    bool destruction_mesh; 

	void Start () 
	{
		transform = TheGameObject.Self.GetComponent<TheTransform>();
        properties = TheGameObject.Self.GetComponent<TheScript>(0);

        ship_parts = new List<TheGameObject>(); 
		need_boom = false; 
		exploted = false; 
	}
	
	void Update ()
	{
		if(need_boom == true && exploted == false)
		{

            TheGameObject current = TheGameObject.Self; 
                     	
			FillPartList(current); 
			SetPartsDirection(); 		
			exploted = true; 
			destroy_timer.Start();
		}

		if(exploted)
		{
			if(destroy_timer.ReadTime() > time_to_destroy)
            {
                DeleteShipParts();
                TheGameObject.Self.SetActive(false); 
            }
				
		} 	
	}

	void FillPartList(TheGameObject go)
	{
		for(int i = 0; i < go.GetChildCount(); i++)
		{			
			ship_parts.Add(go.GetChild(i)); 
		}
	}

	void DeleteShipParts()
	{
		for(int i = 0; i < TheGameObject.Self.GetChildCount(); i++)
		{			
			ship_parts.Remove(TheGameObject.Self.GetChild(i)); 
			ship_parts[i].SetActive(false); 
		}
	}


	void SetPartsDirection()
	{

		for(int i = 0; i < ship_parts.Count ;i++)
		{
			TheVector3 direction = transform.ForwardDirection.Normalized; 

			float randx = TheRandom.RandomRange(-100,100); 
			direction.x = randx; 
			
			float randy = TheRandom.RandomRange(-100,100); 
			direction.y = randy;

			float randz = TheRandom.RandomRange(-100,100); 
			direction.z = randz;
	
			TheRigidBody piece_rb = ship_parts[i].GetComponent<TheRigidBody>(); 
			TheMeshCollider mesh_col = ship_parts[i].GetComponent<TheMeshCollider>(); 
			
			//Disable Colliders 
			ship_parts[i].DestroyComponent(mesh_col); 

			//Modify RigidBody
			piece_rb.Kinematic = false; 
			piece_rb.TransformGO = true; 
			
			direction = direction.Normalized * explosion_v;
			
			//Invert
			float invert = TheRandom.RandomRange(10,20); 

			if(invert >= 15) 
				direction *= -1; 

			piece_rb.SetLinearVelocity(direction.x, direction.y, direction.z);

			float dest_factor = TheRandom.RandomRange(1,50); 

			TheVector3 rotation = direction.Normalized; 

			rotation.x = rotation.x * dest_factor; 
			rotation.y = rotation.y * dest_factor;  
			rotation.z = rotation.z * dest_factor; 
			
			piece_rb.SetAngularVelocity(rotation.x, rotation.y, rotation.z); 		
		}
	}
}