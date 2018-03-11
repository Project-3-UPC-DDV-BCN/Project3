using TheEngine;
using TheEngine.TheConsole;

public class laser 
{
	private TheScript game_manager_scpt; 
	string team; 
	
	
	void Start ()
	{
		team =  TheGameObject.Find("Slave1").tag; 
		game_manager_scpt = TheGameObject.Find("GameManager").GetComponent<TheScript>(0); 
	
	}
	
	void Update () {
		
	}
	
	void TestMethod()
	{
	
		
	}

	void OnCollisionEnter(TheGameObject other_ship)
	{
		
		string enemy_tag = "TIEFIGHTER"; 

		if(team == "Alliance") 
		{
			if(enemy_tag == "TIEFIGHTER" || enemy_tag == "LANDINGCRAFT")
			{
				game_manager_scpt.SetIntField("score_to_inc", 20);
				game_manager_scpt.CallFunction("AddToScore"); 
				game_manager_scpt.SetIntField("score_to_inc", 0);
			}
			else
			{
				game_manager_scpt.SetIntField("score_to_inc", 10);
				game_manager_scpt.CallFunction("SubstractToScore"); 
				game_manager_scpt.SetIntField("score_to_inc", 0);
			}				
		} 
	}
}