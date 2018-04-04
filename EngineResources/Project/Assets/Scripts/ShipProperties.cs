using TheEngine;
using TheEngine.TheConsole; 

public class ShipProperties 
{
	private TheScript destruction_scpt;
	public bool destruction_mesh; 
	
	private TheScript game_manager; 
	
	int hp; 
	int hp_inc; 

	bool is_dead; 

	void Start ()
	{
		hp = 100; 
		hp_inc = 0; 
		is_dead = false;

        string tag = TheGameObject.Self.tag;

        if(tag != "Alliance" && tag != "Empire")
            destruction_scpt = TheGameObject.Self.GetComponent<TheScript>(1);   

		game_manager = TheGameObject.Find("GameManager").GetComponent<TheScript>(); 
	}
	
	void Update () 
	{
		if(hp <= 0 && is_dead == false)
		{		
			bool add_ally_kill = false; 

			if(destruction_mesh == true) 
				destruction_scpt.SetBoolField("need_boom", true);

			is_dead = true; 

			if(TheGameObject.Self.tag == "XWING" || TheGameObject.Self.tag == "YWING")
				add_ally_kill = true; 

			if(game_manager.GetStringField("team") == "empire")
				add_ally_kill = !add_ally_kill; 

			if(add_ally_kill)				
				game_manager.CallFunction("AddAllyKill"); 
			
				
		}

		TheConsole.Log(hp.ToString());
			
	}

	void SubstractHP()
	{
		if(hp - hp_inc <= 0)
			hp = 0; 
		else
			hp -= hp_inc; 
	}

	void AddHP()
	{
		if(hp + hp_inc > 100)
			hp = 100; 
		else
			hp += hp_inc; 
	}

}