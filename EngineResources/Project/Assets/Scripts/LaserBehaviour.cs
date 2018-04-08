using TheEngine;
using TheEngine.TheConsole; 

public class LaserBehaviour 
{

	private TheScript hp_tracker;
	private TheScript game_manager;

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

		if(other_ship_parent.tag == "XWING" || other_ship_parent.tag == "TIEFIGHTER")
		{
			hp_tracker = other_ship_parent.GetComponent<TheScript>(0); 

			TheConsole.Log("sdfdsafsafdasfsda"); 
			
			hp_tracker.SetIntField("inc", 10);  
			hp_tracker.CallFunction("SubstractHP");
			hp_tracker.SetIntField("inc", 0);  
		}


	}
}