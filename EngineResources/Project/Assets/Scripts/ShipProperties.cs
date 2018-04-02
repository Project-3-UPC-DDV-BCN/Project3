using TheEngine;
using TheEngine.TheConsole; 

public class ShipProperties 
{
	private TheScript destruction_scpt;
	public bool destruction_mesh; 
	
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
	}
	
	void Update () 
	{
		if(hp <= 0 && is_dead == false)
		{		
			if(destruction_mesh == true) 
				destruction_scpt.SetBoolField("need_boom", true);

			is_dead = true; 
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