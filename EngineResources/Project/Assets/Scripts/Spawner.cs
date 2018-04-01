using TheEngine;

public class Spawner 
{
	private TheMeshRenderer ship_mesh; 

	public TheGameObject xwing_prf;
    public TheGameObject tiefighter_prf; 
    public TheGameObject ywing_prf; 
	public TheGameObject landcraft_prf; 

	private TheVector3 spawn_position; 

	void Start () 
	{
		spawn_position = TheGameObject.Self.GetComponent<TheTransform>().GlobalPosition; 
		TheGameObject.Self.SetActive(false); 

		if(TheGameObject.Self.tag == "XWING")
		{
			TheGameObject go = TheGameObject.Duplicate(xwing_prf); 
			go.GetComponent<TheTransform>().GlobalPosition = spawn_position; 
		} 
				
		else if(TheGameObject.Self.tag == "TIEFIGHTER") 
		{
            TheGameObject go = TheGameObject.Duplicate(tiefighter_prf);
            go.GetComponent<TheTransform>().GlobalPosition = spawn_position;
        }		
			
		else if(TheGameObject.Self.tag == "LANDINGCRAFT") 
		{
			//Instantiate "LANDINGCRAFT" at position 
		}	

		else if(TheGameObject.Self.tag == "SENTINEL") 
			{
			//Instantiate "SENTINEL" at position 
		}	
	}
	
}