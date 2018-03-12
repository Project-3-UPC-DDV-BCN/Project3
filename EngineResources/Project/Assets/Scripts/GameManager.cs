using TheEngine;
using TheEngine.TheConsole; 

public class GameManager
{

	public TheGameObject show_gametime_go;
	public TheGameObject show_score_go; 

	public int gametime_seconds; 
		
	private float gametime_step; 

	private float timer; 

	//Score Management
	private int score;
	private int score_to_inc; 
	
	private string team; 

	private TheText show_gametime; 
	private TheText show_score; 

	void Init ()
	{
		team = TheGameObject.Find("Slave1").tag; 	
	}

	void Start ()
	{
	 	show_gametime = show_gametime_go.GetComponent<TheText>();
		show_score = show_score_go.GetComponent<TheText>();
		score = 0; 

		timer = (float)gametime_seconds; 
		gametime_step = timer - 1.0f; 	

			 
		show_gametime.Text = GetTimeFromSeconds(gametime_seconds); 
		show_score.Text = score.ToString(); 
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

	void AddToScore()
	{
		score += score_to_inc; 
		
	}

	void SubstractToScore()
	{
		score -= score_to_inc;
		TheConsole.Log("Score substracted");  

		TheConsole.Log(score.ToString()); 
		show_score.Text = score.ToString(); 
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