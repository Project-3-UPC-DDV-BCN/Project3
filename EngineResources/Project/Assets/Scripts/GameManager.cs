using TheEngine;
using System.Collections.Generic; 
using TheEngine.TheConsole; 

public class GameManager
{
	public TheGameObject show_gametime_go;
	public TheGameObject show_score_go; 
	
	public TheGameObject alliance_spawn;
	TheTransform alliance_spawn_trans;
    public TheGameObject empire_spawn;
	TheTransform empire_spawn_trans;

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

	private int ally_killed; 

	private TheText show_gametime; 
	private TheText show_score; 

	public TheGameObject background_music;
	TheAudioSource audio_source;
	
	public TheGameObject slave1;
	TheTransform slave1_trans = null;
	public TheGameObject slave1_audio;
	TheAudioSource slave1_audiosource;

	public TheVector3 position_to_spawn = new TheVector3(0, 0, 0);
	List<TheGameObject> alliance_ships = new List<TheGameObject>();
    List<TheGameObject> empire_ships = new List<TheGameObject>();


	private float player_life = 100;

	void Init ()
	{
		team = TheData.GetString("faction");

		if(team == "no_str")
			team = "rebels";

        TheConsole.Log(team); 
	}

	void Start ()
	{
		player_life = 100;
		
		if(slave1 != null)
			slave1_trans = slave1.GetComponent<TheTransform>();
				
		if(alliance_spawn != null)
			alliance_spawn_trans = alliance_spawn.GetComponent<TheTransform>();
		
		if(empire_spawn != null)
			empire_spawn_trans = empire_spawn.GetComponent<TheTransform>();

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

		if(slave1_audio != null)
			slave1_audiosource = slave1_audio.GetComponent<TheAudioSource>();
		
		if(audio_source != null)
		{
			audio_source.Play("Play_Calm_song");
			audio_source.SetVolume(calm_volume);
		}
			
		ally_killed = 0; 

		score = 0; 
		
		timer = (float)gametime_seconds; 
		gametime_step = timer - 1.0f; 	
	
		background_music_timer = music_timer;

		if (team == "rebels")
        {
            slave1.tag = "Alliance";
            slave1_trans.LocalPosition = new TheVector3(alliance_spawn_trans.LocalPosition.x, alliance_spawn_trans.LocalPosition.y, alliance_spawn_trans.LocalPosition.z);
			slave1_trans.LocalRotation = new TheVector3(180, 0, 0);
        }
        else if (team == "empire")
        {
            slave1.tag = "Empire";
            slave1_trans.LocalPosition = new TheVector3(empire_spawn_trans.LocalPosition.x, empire_spawn_trans.LocalPosition.y, empire_spawn_trans.LocalPosition.z);
        }
	}

	void AddAllyKill()
	{
		ally_killed++;
	}
	
	void Update () 
	{
		timer -= TheTime.DeltaTime; 

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
			
			if(slave1_audiosource != null)
				slave1_audiosource.Stop("Play_Engine");
			
			TheData.AddString("score", score.ToString());
			TheData.AddString("time", GetTimeFromSeconds(gametime_seconds));
			TheApplication.LoadScene("VS3 - EndGameScene");	
		}

		if(ally_killed >= 3 || player_life <= 0)
		{
			TheApplication.LoadScene("VS3 - EndGameScene"); 
		}
	}

	void AddToScore()
	{
		score += score_to_inc;
		
		if(score < 0)
			score = 0; 

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

	void HitPlayer()
	{
		player_life -= 10;
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