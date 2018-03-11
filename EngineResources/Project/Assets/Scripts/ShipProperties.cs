using TheEngine;

public class ShipProperties 
{

	int hp; 
	int hp_inc; 

	int kills; 
	int deaths; 

	void Start ()
	{
		kills = 0; 
		deaths = 0; 
		hp = 100; 
		hp_inc = 0; 
	}
	
	void Update () 
	{
		
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