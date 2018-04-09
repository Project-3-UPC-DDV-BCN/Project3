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
	
	void SetParentTeam(string team)
	{
		parent_team = team;
	}
	
	void Update () {
		
	}

	void OnTriggerEnter(TheGameObject other_ship)
	{

		//Enemy hitç
		
		TheGameObject other_ship_parent = other_ship.GetParent(); 
		
		if(other_ship.tag == "Default")
		{
			TheConsole.Log("NullTag: ALL tags need to be set for laser to interact"); 
			return;
		}
			

		if(other_ship_parent.tag == "XWING" || other_ship_parent.tag == "TIEFIGHTER")
		{
			hp_tracker = other_ship_parent.GetComponent<TheScript>(0); 
			
			TheConsole.Log("HIT EM");
			hp_tracker.SetStringField("last_collided_team", parent_team);
			
			hp_tracker.SetIntField("inc", 10);  
			hp_tracker.CallFunction("SubstractHP");
			hp_tracker.SetIntField("inc", 0);  			
		}
		
		//My Hit 
		else if(other_ship_parent.tag == "Alliance" || other_ship_parent.tag == "Empire")
		{
			TheConsole.Log("HIT MY");
			hp_tracker = other_ship_parent.GetComponent<TheScript>(0);
			hp_tracker.SetIntField("inc", 10);  
			hp_tracker.CallFunction("SubstractHP");
			hp_tracker.SetIntField("inc", 0);			
		}
			


	}
}