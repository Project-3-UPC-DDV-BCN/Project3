using TheEngine;
using TheEngine.TheConsole; 

public class ShipProperties 
{

	private TheScript destruction_scpt;
	
	int hp; 
	int hp_inc; 

	bool is_dead; 

	void Start ()
	{
		hp = 100; 
		hp_inc = 0; 
		is_dead = false; 

		destruction_scpt = TheGameObject.Self.GetChild(0).GetComponent<TheScript>(0);   //The number has to be changed by order
	}
	
	void Update () 
	{
		if(hp <= 0 && is_dead == false)
		{

			destruction_scpt.SetBoolField("need_boom", true); 
			TheConsole.Log("YOU MADE IT"); 
			is_dead = true; 
		}
			
	}

	void SubstractHP()
	{
		hp -= hp_inc; 
	}

	void AddHP()
	{
		hp += hp_inc;
	}

}