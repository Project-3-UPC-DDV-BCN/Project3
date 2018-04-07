using TheEngine;
using TheEngine.TheConsole;

public class laser 
{
	private TheScript game_manager_scpt; 
	private TheScript enemy_properties_scpt; 

    public int enemy_damage;
    public int ally_damage;
	public int slave_damage; 

    bool score_added;  
  
	string team; 

	void Start ()
	{
		TheGameObject game_manager = TheGameObject.Find("GameManager");
		
		if(game_manager != null)
			game_manager_scpt = game_manager.GetComponent<TheScript>(0);

		if(game_manager_scpt != null)
			team = game_manager_scpt.GetStringField("team");

        score_added = false; 

    }
	
	void Update () 
	{
		
	}
	
	void TestMethod()
	{
	
		
	}

	void OnTriggerEnter(TheGameObject other_ship)
	{
		TheGameObject other_parent = other_ship.GetParent();

		TheConsole.Log(other_parent.tag); 

		if(other_parent.tag == "XWING" || other_parent.tag == "TIEFIGHTER")
		{ 	
			enemy_properties_scpt = other_parent.GetComponent<TheScript>(0); 

            //Substract hp to ship
			bool is_enemy = AreEnemies(team, other_parent.tag); 
			int to_inc = 0; 

			if(is_enemy)
				to_inc = enemy_damage; 
			else
				to_inc = ally_damage; 
			
			if(enemy_properties_scpt != null)
			{
				enemy_properties_scpt.SetIntField("hp_inc", to_inc);
				enemy_properties_scpt.CallFunction("SubstractHP"); 
				enemy_properties_scpt.SetIntField("hp_inc", 0);
			

            	//Punish/Reward player if necessary 
            	if (enemy_properties_scpt.GetIntField("hp") <= 0 && score_added == false)
           		{ 
                	int score_inc = GetScoreIncrementByHit(other_parent.tag);
					
					if(game_manager_scpt != null)
					{					
                		game_manager_scpt.SetIntField("score_to_inc", score_inc);
                		game_manager_scpt.CallFunction("AddToScore");
                		game_manager_scpt.SetIntField("score_to_inc", 0);
					}

                	score_added = true; 
            	}
			}
			
			TheGameObject.Self.SetActive(false); 
			
		}
		else if(other_parent.tag == "Alliance")
		{
			
			enemy_properties_scpt = other_parent.GetComponent<TheScript>(0); 

			int to_inc = slave_damage; 
			enemy_properties_scpt.SetIntField("hp_inc", to_inc);
			enemy_properties_scpt.CallFunction("SubstractHP"); 
			enemy_properties_scpt.SetIntField("hp_inc", 0);

		}
	}

    int GetScoreIncrementByHit(string enemy_tag)
    {
        int final_value = 0;

        TheConsole.Log("----");
        TheConsole.Log(team);
        TheConsole.Log(enemy_tag);

        if ((team == "Alliance" && (enemy_tag == "XWING" || enemy_tag == "YWING")) ||
            (team == "Empire" && (enemy_tag == "TIEFIGHTER" || enemy_tag == "LANDCRAFTING")))
            return -30; 

        if (enemy_tag == "XWING" || enemy_tag == "TIEFIGHTER")
            final_value = 25;

        else if (enemy_tag == "YWING" || enemy_tag == "LANDINGCRAFT ")
            final_value = 50;

        if (team == "Empire")
            final_value *= -1;

        TheConsole.Log(final_value.ToString()); 

        return final_value;

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