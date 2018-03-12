using TheEngine;
using TheEngine.TheConsole; 

public class ShipProperties 
{

	private TheScript self_destructor_scpt;
	private TheScript slave_properties; 
	
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
	
		slave_properties = TheGameObject.Find("Slave1").GetComponent<TheScript>(); 

		self_destructor_scpt = TheGameObject.Self.GetComponent<TheScript>(0);   //The number has to be changed by order
	}
	
	void Update () 
	{
		if(hp <= 0 && is_dead == false)
		{
			slave_properties.CallFunction("AddKill"); 

			self_destructor_scpt.SetBoolField("need_boom", true); 
			kills++; 
			is_dead = true; 
		}
			
	}

	void AddKill()
	{
		kills++; 
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

	void OnCollisionEnter()
	{
		hp = 0; 
		TheConsole.Log("Collision");
	}
}