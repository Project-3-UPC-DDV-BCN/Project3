using TheEngine;
using TheEngine.TheConsole; 

public class HP_Tracker {

	public int amount;

	private int inc;

	private bool death;

	private string last_collided_team;	

	private TheScript destruction_scpt; 

	private TheTimer last_hit_timer = new TheTimer();
	private float hit_time = 1.0f;

	void Start () 
	{
		destruction_scpt = TheGameObject.Self.GetComponent<TheScript>(1);
		last_hit_timer.Start(); 
	}
	
	void Update () 
	{
		if(amount <= 0)
		{
			amount = 0; 
			death = true; 
		}
		
	}

	void AddHP()
	{
		amount += inc; 
	}

	void SubstractHP()
	{	
		if(last_hit_timer.ReadTime() > hit_time)
		{
			amount -= inc; 
			last_hit_timer.Start();
		}
	}



}