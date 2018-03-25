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
		ship_parts = new List<TheGameObject>(); 
		need_boom = false; 
		exploted = false; 
	}
	
	void Update ()
	{
		if(TheInput.IsKeyDown("RIGHT_ARROW"))
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
			ship_parts.Add(TheGameObject.Self.GetChild(i)); 
		}
	}


	void SetPartsDirection()
	{



		for(int i = 0; i < ship_parts.Count ;i++)
		{
			TheVector3 direction = transform.ForwardDirection.Normalized; 

			float rand = TheRandom.RandomRange(-5,5); 
			direction.x = rand; 
			
			rand = TheRandom.RandomRange(-5,5); 
			direction.y = rand;

			rand = TheRandom.RandomRange(-5,5); 
			direction.z = rand;
	
			TheRigidBody piece_rb = ship_parts[i].GetComponent<TheRigidBody>(); 
			
			direction = direction.Normalized * explosion_v;

			TheConsole.Log(direction.x);  
			TheConsole.Log(direction.y);  
			TheConsole.Log(direction.z);  

			piece_rb.SetLinearVelocity(direction.x, direction.y, direction.z);		
		}
	}
}