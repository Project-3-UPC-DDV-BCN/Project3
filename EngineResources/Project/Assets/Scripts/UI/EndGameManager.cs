using TheEngine;

public class EndGameManager 
{
	public TheGameObject score_go;	
	public TheGameObject time_go;
	public TheGameObject rounds_go;

	public TheGameObject continue_go;
	public TheGameObject back_to_menu_go;
	public TheGameObject audio_emiter;
	public TheGameObject background_alliance;
	public TheGameObject background_empire;
	public TheGameObject victory_text;
	public TheGameObject lose_text;	

	TheText score_text = null;
	TheText time_text = null;
	TheText rounds_text = null;

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

		if(rounds_go != null)
			rounds_text = rounds_go.GetComponent<TheText>();

		if(continue_go != null)
			continue_rect = continue_go.GetComponent<TheRectTransform>();

		if(back_to_menu_go != null)
			back_to_menu_rect = back_to_menu_go.GetComponent<TheRectTransform>();

		if(audio_emiter != null)
			audio_source = audio_emiter.GetComponent<TheAudioSource>();

		string score = TheData.GetString("score");
		string time = TheData.GetString("time");
		string rounds = TheData.GetString("rounds");

		string side = TheData.GetString("faction");
		int won = TheData.GetInt("won");
		
		mode_played = TheData.GetString("mode");

		//VICTORY & LOSE
		if(victory_text != null && won == 1)
		{
			victory_text.SetActive(true);
			lose_text.SetActive(false);
			
		}else if(lose_text!= null && won == 0)
		{
			victory_text.SetActive(false);
			lose_text.SetActive(true);
		}

		//------------- Text Setting
		if(score_text != null)
			score_text.Text = "" + score;

		if(time_text != null)
			time_text.Text = "" + time;

		if(rounds_text != null)
			rounds_text.Text = "" + rounds;
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
					TheApplication.LoadScene("Alpha1 - TrainingScene");
				}
				else
				{
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