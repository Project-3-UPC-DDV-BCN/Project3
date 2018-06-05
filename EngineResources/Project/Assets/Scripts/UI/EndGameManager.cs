using TheEngine;

public class EndGameManager 
{
	public TheGameObject time_go;
	public TheGameObject turrets_go;
	public TheGameObject ships_go;
	public TheGameObject generators_go;
	public TheGameObject score_go;	
	
	public TheGameObject continue_go;
	public TheGameObject back_to_menu_go;
	public TheGameObject audio_emiter;
	public TheGameObject background_alliance;
	public TheGameObject background_empire;
	public TheGameObject victory_text;
	public TheGameObject lose_text;	

	public int multiply_turrets_by = 100;
	public int multiply_ships_by = 200;
	public int multiply_generators_by = 500;
	
	
	TheText time_text = null;
	TheText turrets_text = null;
	TheText ships_text = null;	
	TheText generators_text = null;
	TheText score_text = null;
	
	TheRectTransform continue_rect = null;
	TheRectTransform back_to_menu_rect = null;
	TheAudioSource audio_source = null;

	bool sound_over = false;
	bool sound_pressed = false;
	
	string mode_played = "Campaign";

	void Start () 
	{
		if(score_go != null)
			score_text = score_go.GetComponent<TheText>();

		if(time_go != null)
			time_text = time_go.GetComponent<TheText>();

		if(turrets_go != null)
			turrets_text = turrets_go.GetComponent<TheText>();
		if(ships_go != null)
			ships_text = ships_go.GetComponent<TheText>();
		if(generators_go != null)
			generators_text = generators_go.GetComponent<TheText>();
		
		if(continue_go != null)
			continue_rect = continue_go.GetComponent<TheRectTransform>();

		if(back_to_menu_go != null)
			back_to_menu_rect = back_to_menu_go.GetComponent<TheRectTransform>();

		if(audio_emiter != null)
			audio_source = audio_emiter.GetComponent<TheAudioSource>();

		string side = TheData.GetString("faction");
		int won = TheData.GetInt("won");
		
		mode_played = TheData.GetString("mode");

		//VICTORY & LOSE
		if(victory_text != null && won == 1)
		{
			victory_text.SetActive(true);
			lose_text.SetActive(false);
			
		}
		else if(lose_text!= null && won == 0)
		{
			victory_text.SetActive(false);
			lose_text.SetActive(true);
		}

		if(audio_source!= null)
			audio_source.Play("Play_Menu_song");

		if(mode_played == "training")
		{
			string score = TheData.GetString("score");
			int time = TheData.GetInt("time");
			string rounds = TheData.GetString("rounds");
			int ships = TheData.GetInt("ships");
			
			if(ships_go != null)
				ships_text.Text = "" + ships;
			if(turrets_go != null)
				turrets_text.Text = "" + rounds;

			if(score_text != null)
				score_text.Text = "" + score;
			
			if(time_text != null)
			{	
				int hours = time / 3600;
				int seconds = time % 3600;
				int minutes = seconds / 60;
				seconds %= 60;
		
				string hours_str = ""+hours;
				string minutes_str = ""+minutes;
				if (minutes < 10)
					minutes_str = "0"+minutes;
				string seconds_str = ""+seconds;
				if (seconds < 10)
					seconds_str = "0"+seconds;
			
				time_text.Text = ""+hours_str + ":" + minutes_str + ":" + seconds_str;
			}
		}
		else
		{
			int time = TheData.GetInt("time");
			int ships = TheData.GetInt("ships") * multiply_ships_by;	
			int generators = TheData.GetInt("generators") * multiply_generators_by;
			int turrets = TheData.GetInt("turrets") * multiply_turrets_by;
			int score = turrets + generators + ships;
			
			//------------- Text Setting
			if(score_text != null)
				score_text.Text = "" + score;

			if(turrets_go != null)
				turrets_text.Text = "" + turrets;
			if(ships_go != null)
				ships_text.Text = "" + ships;
			if(generators_go != null)
				generators_text.Text = "" + generators;
			if(time_text != null)
			{	
				int hours = time / 3600;
				int seconds = time % 3600;
				int minutes = seconds / 60;
				seconds %= 60;
		
				string hours_str = ""+hours;
				string minutes_str = ""+minutes;
				if (minutes < 10)
					minutes_str = "0"+minutes;
				string seconds_str = ""+seconds;
				if (seconds < 10)
					seconds_str = "0"+seconds;
			
				time_text.Text = ""+hours_str + ":" + minutes_str + ":" + seconds_str;
			}
		}
	}
	
	void Update () 
	{
		if(continue_rect != null)
		{
			if(continue_rect.OnClickUp)
			{
				sound_pressed = true;
				if(mode_played == "training")
				{
					audio_source.Stop("Play_Menu_song");
					TheApplication.LoadScene("Alpha1 - TrainingScene");
					
				}
				else
				{
					audio_source.Stop("Play_Menu_song");
					TheApplication.LoadScene("Alpha1 - Level1Scene");
					
				}
			}

			if(continue_rect.OnMouseEnter)
			{
				sound_over = true;
			}
		}

		if(back_to_menu_rect != null)
		{
			if(back_to_menu_rect.OnClickUp)
			{
				sound_pressed = true;
				audio_source.Stop("Play_Menu_song");
				TheApplication.LoadScene("Alpha1 - MainMenuScene");
			}

			if(back_to_menu_rect.OnMouseEnter)
			{
				sound_over = true;
			}
		}

		if(sound_over)
		{
			if(audio_source != null)
				audio_source.Play("Play_hover");
			
			sound_over = false;
		}

		if(sound_pressed)
		{
			if(audio_source != null)
				audio_source.Play("Play_click");
		
			sound_pressed = false;
		}
	}
}