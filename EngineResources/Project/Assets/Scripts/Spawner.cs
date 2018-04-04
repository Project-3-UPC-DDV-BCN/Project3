using TheEngine;

public class Spawner 
{	
	public string xwing_prefab_name;
	public string tie_fighter_prefab_name;
	
	private TheMeshRenderer ship_mesh; 
	
	private TheVector3 spawn_position; 
	private TheGameObject ship_to_spawn;  

	void Start () 
	{
			
		spawn_position = TheGameObject.Self.GetComponent<TheTransform>().LocalPosition; 
		TheGameObject.Self.SetActive(false); 

		if(TheGameObject.Self.tag == "XWING")
		{
			ship_to_spawn = TheResources.LoadPrefab(xwing_prefab_name);
			if(ship_to_spawn != null)
			{
				ship_to_spawn.GetComponent<TheTransform>().GlobalPosition = spawn_position; 
			}	
		} 
				
		else if(TheGameObject.Self.tag == "TIEFIGHTER") 
		{
			ship_to_spawn = TheResources.LoadPrefab(tie_fighter_prefab_name);
			if(ship_to_spawn != null)
			{
            	ship_to_spawn.GetComponent<TheTransform>().LocalPosition = spawn_position;
			}
        }			
	}
	
}