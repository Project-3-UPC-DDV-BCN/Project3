using TheEngine;
using TheEngine.TheConsole; 

public class CollisionAction {

	private TheScript game_manager_scpt; 
	private TheScript ship_properties_scpt; 

	void Start () 
	{
		game_manager_scpt = TheGameObject.Find("GameManager").GetComponent<TheScript>(0);
		ship_properties_scpt = TheGameObject.Self.GetComponent<TheScript>(1);  
	}
	
	void Update () {
		
	}

	void OnCollisionEnter(TheGameObject bullet)
	{
		
		TheConsole.Log("collided"); 

		string my_team = game_manager_scpt.GetStringField("tag"); 
		string ship_hit_team = TheGameObject.Self.tag; 

		if(my_team == "Alliance")
		{
			if(ship_hit_team == "Empire")
			{
				game_manager_scpt.SetIntField("score_to_inc", 20);
				game_manager_scpt.CallFunction("AddToScore"); 
				game_manager_scpt.SetIntField("score_to_inc", 0);

				//ENEMY HP
				ship_properties_scpt.SetIntField("hp_inc", 20);
				ship_properties_scpt.CallFunction("SubstractHP"); 
				ship_properties_scpt.SetIntField("hp_inc", 0);
			}
			else if (ship_hit_team == "Alliance")
			{
				game_manager_scpt.SetIntField("score_to_inc", 10);
				game_manager_scpt.CallFunction("SubstractToScore"); 
				game_manager_scpt.SetIntField("score_to_inc", 0);

				//ENEMY HP
				ship_properties_scpt.SetIntField("hp_inc", 10);
				ship_properties_scpt.CallFunction("SubstractHP"); 
				ship_properties_scpt.SetIntField("hp_inc", 0);
			}

		}

	 
	}
}