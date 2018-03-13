using TheEngine;
using TheEngine.TheConsole; 

public class GameManager
{

	public TheGameObject show_gametime_go;
	public TheGameObject show_score_go; 

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

		background_music_timer = music_timer;
		
		show_gametime.Text = GetTimeFromSeconds(gametime_seconds); 
		show_score.Text = score.ToString();

		audio_source = background_music.GetComponent<TheAudioSource>();
		audio_source.Play("Play_Calm_song");
		audio_source.SetVolume(calm_volume);
		slave1_audio = slave1.GetComponent<TheAudioSource>();
	}
	
	
	void Update () 
	{
		timer -= TheTime.DeltaTime;

		if (TheInput.GetControllerJoystickMove(0,"LEFT_TRIGGER") >= 20000 && !calm_combat)
		{
			audio_source.SetVolume(combat_volume);
			audio_source.Stop("Play_Calm_song");
			audio_source.Play("Play_Combat_song");
			calm_combat = true;
		}
		
		if (calm_combat)
		{
			background_music_timer -= TheTime.DeltaTime;
			
			if(background_music_timer < 0.0f)
			{
				audio_source.SetVolume(calm_volume);
				background_music_timer = music_timer;
				audio_source.Stop("Play_Combat_song");
				audio_source.Play("Play_Calm_song");
				calm_combat = false;
			}
		}
	
		if(timer < gametime_step)
		{
			gametime_step = timer - 1.0f;
			gametime_seconds--;  

			show_gametime.Text = GetTimeFromSeconds(gametime_seconds);	
		}

		if(gametime_seconds <= 0) 
		{
			audio_source.Stop("Play_Calm_song");
			audio_source.Stop("Play_Combat_song");
			slave1_audio.Stop("Play_Engine");
			TheApplication.LoadScene("VS2-UI");	
		}	 		

	}

	void AddToScore()
	{
		score += 1; 
		show_score.Text = score.ToString();
		
	}

	void SubstractToScore()
	{
		if(score - score_to_inc > 0)
		{
			score -= score_to_inc; 
			show_score.Text = score.ToString(); 	
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