using TheEngine;
using TheEngine.TheConsole; 

public class LaserBehaviour 
{

	private TheScript hp_tracker;
	private TheScript other_ship_properties;
	private TheScript game_manager;
	
	public string parent_team; 

	void Start () 
	{
		hp_tracker = null; 
	}
	
	void Update () {
		
	}

	void OnTriggerEnter(TheGameObject other_ship)
	{

		//Enemy hit
		
		TheGameObject other_ship_parent = other_ship.GetParent(); 

		if(other_ship_parent != null)
		{
			TheConsole.Log("se hiteo algo");
			TheConsole.Log(other_ship_parent.tag);
			if(other_ship_parent.tag == "XWING" || other_ship_parent.tag == "TIEFIGHTER")
			{
				TheConsole.Log("hiteo la ia");
				hp_tracker = other_ship_parent.GetComponent<TheScript>(0); 

				if(hp_tracker != null)
				{
					hp_tracker.SetStringField("last_collided_team", parent_team);
			
					hp_tracker.SetIntField("inc", 10);  
					hp_tracker.CallFunction("SubstractHP");
					hp_tracker.SetIntField("inc", 0);  		
				}	
			}
		
			//My Hit 
			else if(other_ship_parent.tag == "Alliance" || other_ship_parent.tag == "Empire")
			{
				game_manager.CallFunction("HitPlayer");
				TheConsole.Log("HIT MY");	
			}
		}
	}
}