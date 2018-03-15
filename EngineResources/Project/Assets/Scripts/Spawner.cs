using TheEngine;

public class Spawner 
{
	private TheMeshRenderer ship_mesh; 

	private TheGameObject xwing_prf; 
	private TheGameObject ywing_prf; 
	private TheGameObject landcraft_prf; 
	private TheGameObject sentinel_prf; 

	private spawn_position; 

	void Start () 
	{
		ship_mesh = TheGameObject.Self.GetComponent<TheMeshRenderer>();
		spawn_position = TheGameObject.Self.GetComponent<TheTransform>().GlobalPosition; 
		TheGameObject.Self.SetActive(false); 

		if(TheGameObject.Self.GetTag() == "XWING") 
			//Instantiate "XWING" at position
		
		else if(TheGameObject.Self.GetTag() == "TIEFIGHTER") 
			//Instantiate "TIEFIGHTER" at position 

		else if(TheGameObject.Self.GetTag() == "LANDINGCRAFT") 
			//Instantiate "LANDINGCRAFT" at position 

		else if(TheGameObject.Self.GetTag() == "SENTINEL") 
			//Instantiat e"SENTINEL" at position 
	}
	
	void Update () 
	{
		
	}
}