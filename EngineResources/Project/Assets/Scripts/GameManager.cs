using TheEngine;
using TheEngine.TheConsole; 

public class GameManager
{

	public TheGameObject show_gametime_go;
	public int gametime_seconds; 
		
	private float gametime_step; 
	private float gametime_miliseconds; 
	private TheText show_gametime; 

	void Start ()
	{
	 	show_gametime = show_gametime_go.GetComponent<TheText>();
		gametime_miliseconds = gametime_seconds * 1000; 
		gametime_step = 0; 		
		 
		show_gametime.Text = GetTimeFromSeconds(gametime_seconds); 
	}
	
	void Update () 
	{
		gametime_miliseconds -= TheTime.DeltaTime;
		gametime_step += TheTime.DeltaTime; 

		if(gametime_step > 1000)
		{
			gametime_step = 0;
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
		int division = (int)gametime_seconds/60; 
		float reminder = gametime_seconds % 60;

		string new_time = division.ToString(); 
		new_time += ":";

		if(reminder < 10) 
			new_time += "0";

		new_time += reminder.ToString(); 
		 
		return new_time; 
	}

	
}