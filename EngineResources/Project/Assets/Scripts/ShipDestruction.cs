using TheEngine;
using TheEngine.TheConsole; 
using System.Collections.Generic; 
using TheEngine.TheMath;

public class ShipDestruction 
{

	public float explosion_v; 
	public float time_to_destroy; 
	public bool start_automatic; 

	private TheTimer destroy_timer = new TheTimer();

	List<TheGameObject> ship_parts; 

	TheTransform transform; 
	string ship_tag; 

	bool need_boom;
	bool exploted;
    bool destruction_mesh; 

	void Start () 
	{
		transform = TheGameObject.Self.GetComponent<TheTransform>();

        ship_parts = new List<TheGameObject>(); 
		need_boom = false; 
		exploted = false; 
	}
	
	void Update ()
	{
		if(need_boom == true && exploted == false || start_automatic == true && exploted == false)
		{                     	
			PlayDestruction(); 
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

	void PlayDestruction()
	{
		FillPartList(); 
		SetPartsDirection(); 		
		exploted = true; 
		destroy_timer.Start();
	}

    TheGameObject SwapModel()
    {
        TheGameObject obj_to_ret = null; 
        TheGameObject.Self.SetActive(false);

        if (ship_tag == "TIEFIGHTER")
        {
            TheVector3 ship_pos = transform.GlobalPosition; 

            TheGameObject.Self.SetActive(false);
            TheGameObject ship_to_spaw = TheResources.LoadPrefab("TieFighterDestruct");
            obj_to_ret = TheGameObject.Duplicate(ship_to_spaw);
            obj_to_ret.GetComponent<TheTransform>().GlobalPosition = ship_pos;
        }

        return obj_to_ret; 
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