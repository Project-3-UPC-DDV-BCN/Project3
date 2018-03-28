using TheEngine;
using TheEngine.TheConsole; 
using System.Collections.Generic; 
using TheEngine.TheMath;

public class ShipDestruction 
{

	public float explosion_v; 
	public float time_to_destroy; 

	private float destroy_timer; 

	List<TheGameObject> ship_parts; 

	TheTransform transform; 
	string ship_tag; 

	bool need_boom;
	bool exploted;  

	void Start () 
	{
		transform = TheGameObject.Self.GetComponent<TheTransform>(); 
		ship_parts = new List<TheGameObject>(); 
		need_boom = false; 
		exploted = false; 
		destroy_timer = 0; 
	}
	
	void Update ()
	{
		if(TheInput.IsKeyDown("RIGHT_ARROW"))
		{			
			FillPartList(); 
			SetPartsDirection(); 		
			exploted = true; 
		}

		if(exploted)
		{
			destroy_timer++; 

			if(destroy_timer > time_to_destroy * 100)
				DeleteShipParts(); 
		}

		TheConsole.Log("---");
		TheConsole.Log(destroy_timer); 
		TheConsole.Log(time_to_destroy);  	
	}

	void FillPartList()
	{
		for(int i = 0; i < TheGameObject.Self.GetChildCount(); i++)
		{			
			ship_parts.Add(TheGameObject.Self.GetChild(i)); 
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

			float randx = TheRandom.RandomRange(-5,5); 
			direction.x = randx; 
			
			float randy = TheRandom.RandomRange(-5,5); 
			direction.y = randy;

			float randz = TheRandom.RandomRange(-5,5); 
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
			piece_rb.SetRotation(20,20,20); 		
		}
	}
}