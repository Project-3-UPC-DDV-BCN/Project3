using TheEngine;
using TheEngine.TheConsole; 

public class GameManager
{

	public TheGameObject show_gametime_go;
	public int gametime_seconds; 
		
	private float gametime_step; 
	private float timer; 
	private TheText show_gametime; 

	void Start ()
	{
	 	show_gametime = show_gametime_go.GetComponent<TheText>();
		timer = (float)gametime_seconds; 
		gametime_step = timer - 1.0f; 		
		 
		show_gametime.Text = GetTimeFromSeconds(gametime_seconds); 
	}
	
	void Update () 
	{
		timer -= TheTime.DeltaTime; 

		if(timer < gametime_step)
		{
			gametime_step = timer - 1.0f;
			gametime_seconds--;  

			show_gametime.Text = GetTimeFromSeconds(gametime_seconds);	
		}

		if(gametime_seconds == 0) 
		{
			//LOOSE
		}	 		

	}

	string GetTimeFromSeconds(int seconds)
	{
		int division = seconds/60; 
		float reminder = seconds % 60;

		string new_time = division.ToString(); 
		new_time += ":";

		if(reminder < 10) 
			new_time += "0";

		new_time += reminder.ToString(); 
		 
		return new_time; 
	}

	
}