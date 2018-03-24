using TheEngine;
using TheEngine.TheConsole;

public class laser 
{
	private TheScript game_manager_scpt; 
	private TheScript enemy_properties_scpt; 
	string team; 

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


		TheConsole.Log("Entered"); 

		TheGameObject parent = other_ship.GetParent(); 	
		enemy_properties_scpt = parent.GetComponent<TheScript>(0); 

		bool is_enemy = AreEnemies(team, parent.tag); 

		if(other_ship != null)
		{
			int to_inc = 0; 

			if(is_enemy)
				to_inc = 20; 
			else
				to_inc = 10; 
			
			enemy_properties_scpt.SetIntField("hp_inc", to_inc);
			enemy_properties_scpt.CallFunction("SubstractHP"); 
			enemy_properties_scpt.SetIntField("hp_inc", 0); 
		}

	}

	bool AreEnemies(string team1, string team2)
	{
		bool return_value; 

		if(team2 == "XWING" || team2 == "YWING")
			return_value = false; 
		else
			return_value = true; 

		if(team1 == "EMPIRE")
			return_value = !return_value;

		return return_value; 
	}
}