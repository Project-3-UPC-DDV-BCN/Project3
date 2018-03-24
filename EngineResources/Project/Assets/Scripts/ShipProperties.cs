using TheEngine;
using TheEngine.TheConsole; 

public class ShipProperties 
{

	private TheScript destruction_scpt;
	private TheScript game_manager;

	public TheGameObject game_manager_obj; 
	
	int hp; 
	int hp_inc; 

	bool is_dead; 

	void Start ()
	{
		hp = 100; 
		hp_inc = 0; 
		is_dead = false; 
		
		game_manager_obj = TheGameObject.Find("GameeManager"); 
		game_manager = game_manager_obj.GetComponent<TheScript>(0);
		destruction_scpt = TheGameObject.Self.GetComponent<TheScript>(1);   //The number has to be changed by order
	}
	
	void Update () 
	{
		if(hp <= 0 && is_dead == false)
		{

			destruction_scpt.SetBoolField("need_boom", true);

			game_manager.SetIntField("score_to_inc", 100); 
			game_manager.CallFunction("AddScore");  
			game_manager.SetIntField("score_to_inc", 0);

			TheConsole.Log("YOU MADE IT"); 
			is_dead = true; 
		}
			
	}

	void SubstractHP()
	{
		hp -= hp_inc; 
		TheConsole.Log(hp.ToString());
	}

	void AddHP()
	{
		hp += hp_inc;
	}

}