using TheEngine;
using TheEngine.TheConsole;

public class laser 
{
	private TheScript game_manager_scpt; 
	string team; 

	private TheScript ship_hit_scpt; 
	
	void Start ()
	{
		team =  TheGameObject.Find("Slave1").tag; 
		game_manager_scpt = TheGameObject.Find("GameManager").GetComponent<TheScript>(0);
	}
	
	void Update () 
	{
		
	}
	
	void TestMethod()
	{
	
		
	}

	void OnCollisionEnter(TheGameObject other_ship)
	{
		string enemy_tag = "TIEFIGHTER"; 
		TheConsole.Log("Collided"); 

		ship_hit_scpt = other_ship.GetComponent<TheScript>(0); //Num has to be change for the order  

		if(team == "Alliance") 
		{
			if(enemy_tag == "TIEFIGHTER" || enemy_tag == "LANDINGCRAFT")
			{
				//PLAYER SCORE
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
	}
}