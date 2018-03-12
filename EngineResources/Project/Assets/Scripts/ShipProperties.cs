using TheEngine;

public class ShipProperties 
{

	private TheScript self_destructor_scpt; 
	
	int hp; 
	int hp_inc; 

	int kills; 
	int deaths; 

	bool is_dead; 

	void Start ()
	{
		kills = 0; 
		deaths = 0; 
		hp = 100; 
		hp_inc = 0; 
		is_dead = false; 
	
		self_destructor_scpt = TheGameObject.Self.GetComponent<TheScript>(0);   //The number has to be changed by order
	}
	
	void Update () 
	{
		if(hp <= 0)
		{
			self_destructor_scpt.SetBoolField("need_boom", true); 
			kills++; 
			is_dead = true; 
		}
			
	}

	int GetKills()
	{
		return kills; 
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