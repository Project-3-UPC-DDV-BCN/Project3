using TheEngine;

public class Spawner 
{
	private TheMeshRenderer ship_mesh; 

	private TheVector3 spawn_position; 
	private TheGameObject ship_to_spawn;  

	void Start () 
	{
			
		spawn_position = TheGameObject.Self.GetComponent<TheTransform>().GlobalPosition; 
		TheGameObject.Self.SetActive(false); 

		if(TheGameObject.Self.tag == "XWING")
		{
			ship_to_spawn = TheResources.LoadPrefab("X_Wing(12)");
			TheGameObject go = TheGameObject.Duplicate(ship_to_spawn); 
			go.GetComponent<TheTransform>().GlobalPosition = spawn_position; 
		} 
				
		else if(TheGameObject.Self.tag == "TIEFIGHTER") 
		{
			ship_to_spawn = TheResources.LoadPrefab("TieFighterOnePiece");
            TheGameObject go = TheGameObject.Duplicate(ship_to_spawn);
            go.GetComponent<TheTransform>().GlobalPosition = spawn_position;
        }			
	}
	
}