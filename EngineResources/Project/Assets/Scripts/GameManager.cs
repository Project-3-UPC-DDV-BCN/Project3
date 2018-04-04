using TheEngine;
using System.Collections.Generic; 
using TheEngine.TheConsole; 

public class GameManager
{
	public TheGameObject show_gametime_go;
	public TheGameObject show_score_go; 
	
	public TheVector3 alliance_spawn = new TheVector3(0, 0, 0);
	public TheVector3 empire_spawn = new TheVector3(0, 0, 0);

	public int gametime_seconds; 
		
	private float gametime_step; 

	private float timer; 
	
	public float music_timer = 15;
	private float background_music_timer;
	private bool calm_combat = false;
	
	public int calm_volume = 35;
	public int combat_volume = 20;

	//Score Management
	private int score;
	private int score_to_inc; 
	
	private string team; 

	private TheText show_gametime; 
	private TheText show_score; 

	public TheGameObject background_music;
	TheAudioSource audio_source;
	
	public TheGameObject slave1;
	TheAudioSource slave1_audio;

	public TheVector3 position_to_spawn = new TheVector3(0, 0, 0);
	List<TheGameObject> alliance_ships = new List<TheGameObject>();
    List<TheGameObject> empire_ships = new List<TheGameObject>(); 

	void Init ()
	{
		team = TheData.GetString("faction");  

        TheConsole.Log(team); 
	}

	void Start ()
	{
		if(show_gametime_go != null)
	 		show_gametime = show_gametime_go.GetComponent<TheText>();

		if(show_score_go != null)
			show_score = show_score_go.GetComponent<TheText>();
		
		if(show_gametime != null)
			show_gametime.Text = GetTimeFromSeconds(gametime_seconds); 

		if(show_score != null)
			show_score.Text = score.ToString();

		if(background_music != null)
			audio_source = background_music.GetComponent<TheAudioSource>();

		if(audio_source != null)
		{
			audio_source.Play("Play_Calm_song");
			audio_source.SetVolume(calm_volume);
		}

		if(slave1 != null)
		{
			slave1_audio = slave1.GetComponent<TheAudioSource>();
		}
			

		score = 0; 
		
		timer = (float)gametime_seconds; 
		gametime_step = timer - 1.0f; 	
	
		background_music_timer = music_timer;
	}
	
	
	void Update () 
	{
		timer -= TheTime.DeltaTime;
		TheConsole.Log(team); 

		if (TheInput.GetControllerJoystickMove(0,"LEFT_TRIGGER") >= 20000 && !calm_combat)
		{
			if(audio_source != null)
			{
				audio_source.SetVolume(combat_volume);
				audio_source.Stop("Play_Calm_song");
				audio_source.Play("Play_Combat_song");
			}
			calm_combat = true;
		}
		
		if (calm_combat)
		{
			background_music_timer -= TheTime.DeltaTime;
			
			if(background_music_timer < 0.0f)
			{	
				if(audio_source != null)
				{
					audio_source.SetVolume(calm_volume);
					audio_source.Stop("Play_Combat_song");
					audio_source.Play("Play_Calm_song");
				}

				background_music_timer = music_timer;

				calm_combat = false;
			}
		}
	
		if(timer < gametime_step)
		{
			gametime_step = timer - 1.0f;
			gametime_seconds--;  

			if(show_gametime != null)
				show_gametime.Text = GetTimeFromSeconds(gametime_seconds);
		}

		if(gametime_seconds <= 0) 
		{
			if(audio_source != null)
			{
				audio_source.Stop("Play_Calm_song");
				audio_source.Stop("Play_Combat_song");
			}
			
			if(slave1_audio != null)
				slave1_audio.Stop("Play_Engine");

			TheApplication.LoadScene("VS3 - MainMenu");	
		}
	}

	void AddToScore()
	{
		score += score_to_inc;

		if(show_score != null)
       		show_score.Text = score.ToString();
    }


	void SubstractToScore()
	{
		if(score - score_to_inc > 0)
			score -= score_to_inc;

		if(show_score != null)
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

	void AddToAlliance()
	{	
		TheGameObject new_go = TheResources.LoadPrefab("AllianceShip");
			
		if(new_go != null)
		{
			TheTransform trans = new_go.GetComponent<TheTransform>();

			if(trans != null)		
			{	
				trans.LocalPosition = position_to_spawn;
			}

			alliance_ships.Add(new_go);
		}
	}

	void AddToEmpire()
	{
	
		TheGameObject new_go = TheResources.LoadPrefab("RebelsShip");
			
		if(new_go != null)
		{
			TheTransform trans = new_go.GetComponent<TheTransform>();

			if(trans != null)		
			{	
				trans.LocalPosition = new TheVector3(position_to_spawn.x, position_to_spawn.y, position_to_spawn.z);
			}

			empire_ships.Add(new_go);
		} 
	}
}