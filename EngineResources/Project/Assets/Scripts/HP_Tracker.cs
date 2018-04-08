using TheEngine;
using TheEngine.TheConsole; 

public class HP_Tracker {

	public int amount;
	private int inc;

	private bool death;   

	private TheScript destruction_scpt; 

	void Start () 
	{
		destruction_scpt = TheGameObject.Self.GetComponent<TheScript>(1); 
	}
	
	void Update () 
	{
		if(amount <= 0)
		{
			amount = 0; 
			death = true; 
			destruction_scpt.SetBoolField("need_boom", true); 
		}

	 TheConsole.Log(amount); 
		
	}

	void AddHP()
	{
		amount += inc; 
	}

	void SubstractHP()
	{
		amount -= inc; 
	}



}