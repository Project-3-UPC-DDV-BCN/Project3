using TheEngine;
using System.Collections.Generic; 
using TheEngine.TheConsole; 

public class GameManager
{
	private string team = "no_str"; 

	public int game_time = 120;
	private TheTimer game_timer = new TheTimer();

	private int score = 0;

	public TheGameObject gametime_go;
	private TheText gametime_text = null; 
	public TheGameObject score_go; 
	private TheText score_text = null; 
	
	public TheGameObject alliance_spawn;
	TheTransform alliance_spawn_trans = null;
    public TheGameObject empire_spawn;
	TheTransform empire_spawn_trans = null;
	
	public float music_timer = 15;
	private float background_music_timer = 0.0f;
	private bool calm_combat = false;
	
	public int calm_volume = 35;
	public int combat_volume = 20;

	public TheGameObject background_music;
	TheAudioSource audio_source = null;
	
	public TheGameObject slave1;
	public TheGameObject slave1_audio;
	TheTransform slave1_trans = null;
	TheAudioSource slave1_audiosource = null;

	List<TheGameObject> alliance_ships = new List<TheGameObject>();
    List<TheGameObject> empire_ships = new List<TheGameObject>();

	void Init ()
	{
		team = TheData.GetString("faction");

		if(team == "no_str")
			team = "rebels";

        TheConsole.Log(team); 
	}

	void Start ()
	{
		if(slave1 != null)
			slave1_trans = slave1.GetComponent<TheTransform>();
				
		if(alliance_spawn != null)
			alliance_spawn_trans = alliance_spawn.GetComponent<TheTransform>();
		
		if(empire_spawn != null)
			empire_spawn_trans = empire_spawn.GetComponent<TheTransform>();

		if(gametime_go != null)
	 		gametime_text = gametime_go.GetComponent<TheText>();

		if(score_go != null)
			score_text = score_go.GetComponent<TheText>();
		
		if(gametime_text != null)
			gametime_text.Text = GetTimeFromSeconds(game_time); 

		if(score_text != null)
			score_text.Text = score.ToString();

		if(background_music != null)
			audio_source = background_music.GetComponent<TheAudioSource>();

		if(slave1_audio != null)
			slave1_audiosource = slave1_audio.GetComponent<TheAudioSource>();
		
		if(audio_source != null)
		{
			audio_source.Play("Play_Calm_song");
			audio_source.SetVolume(calm_volume);
		}
			
		background_music_timer = music_timer;

		game_timer.Start();
	}
	
	void Update () 
	{
		UpdateAudio();

		UpdateTimePointsTexts();
		
	}
	
	void AddAllianceShip(TheGameObject add)
	{
		alliance_ships.Add(add);
	}

	void AddEmpireShip(TheGameObject add)
	{
		empire_ships.Add(add);
	}

	void RemoveAllianceShip(TheGameObject remove)
	{
		alliance_ships.Remove(remove);
	}

	void RemoveEmpireShip(TheGameObject remove)
	{
		empire_ships.Remove(remove);
	}
	
	int AllianceShipsCount()
	{
		return alliance_ships.Count;
	}
	
	int EmpireShipsCount()
	{
		return empire_ships.Count;
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

	void UpdateTimePointsTexts()
	{
		int time_to_end = (int)((float)game_time - game_timer.ReadTime());
		
		if(gametime_text != null)
			gametime_text.Text = GetTimeFromSeconds(time_to_end); 

		if(score_text != null)
			score_text.Text = score.ToString();
	}

	void UpdateAudio()
	{
		if (TheInput.GetControllerJoystickMove(0, "LEFT_TRIGGER") >= 20000 && !calm_combat)
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
	}
}