using TheEngine;
using TheEngine.TheConsole;

public class laser 
{
	private TheScript game_manager_scpt; 
	string team; 

	private TheScript ship_hit_scpt; 
	
	void Start ()
	{
		
		game_manager_scpt = TheGameObject.Find("GameManager").GetComponent<TheScript>(0);
		team = game_manager_scpt.GetStringField("team"); 
	}
	
	void Update () 
	{
		
	}
	
	void TestMethod()
	{
	
		
	}

	void OnCollisionEnter(TheGameObject other_ship)
	{
		if(other_ship != null)
		{
			game_manager_scpt.CallFunction("AddToScore");
			other_ship.SetActive(false);
			TheGameObject.Self.SetActive(false);
		}
			TheConsole.Log("Collision");
		/*if(other_ship != null)
		{	
			string enemy_tag = other_ship.tag; 
			TheConsole.Log("Collided"); 
			TheConsole.Log(other_ship.name); 

			ship_hit_scpt = other_ship.GetComponent<TheScript>(); //Num has to be change for the order  

			if(team == "Alliance") 
			{
				TheConsole.Log("You are alliance"); 

				if(enemy_tag == "Empire")
				{
					game_manager_scpt.SetIntField("score_to_inc", 20);
					game_manager_scpt.CallFunction("AddToScore"); 
					game_manager_scpt.SetIntField("score_to_inc", 0);

					//ENEMY HP
					ship_hit_scpt.SetIntField("hp_inc", 20);
					ship_hit_scpt.CallFunction("SubstractHP"); 
					ship_hit_scpt.SetIntField("hp_inc", 0); 
				}
				else
				{
					game_manager_scpt.SetIntField("score_to_inc", 10);
					game_manager_scpt.CallFunction("SubstractToScore"); 
					game_manager_scpt.SetIntField("score_to_inc", 0);

					ship_hit_scpt.SetIntField("hp_inc", 10);
					ship_hit_scpt.CallFunction("SubstractHP"); 
					ship_hit_scpt.SetIntField("hp_inc", 0); 
				}				
			} 
		}*/
	}
}