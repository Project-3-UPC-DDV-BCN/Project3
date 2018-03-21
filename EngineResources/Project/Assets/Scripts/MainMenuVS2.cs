using TheEngine;
using TheEngine.TheConsole;

public class MainMenuVS2 
{
	public TheGameObject menu_go;
	public TheGameObject side_selection_go;

	public TheGameObject campaign_button_go;
	public TheGameObject settings_button_go;
	public TheGameObject exit_button_go;
	public TheGameObject explanation_text_go;
	public TheGameObject loading_text_go;
	public TheGameObject side_selection_continue_go;

	TheRectTransform campaign_rect = null;
	TheRectTransform settings_rect = null;
	TheRectTransform exit_rect = null;
	TheRectTransform continue_rect = null;
	TheText	explanation_text = null;	
	
	TheAudioSource menu_audio_source = null;

	bool pressed_sound = false;
	bool over_sound = false;

	void Start () 
	{
		if(menu_go != null)
			menu_go.SetActive(true);
	
		if(side_selection_go != null)
			side_selection_go.SetActive(false);
	
		if(campaign_button_go != null)
			campaign_rect = campaign_button_go.GetComponent<TheRectTransform>();
		
		if(settings_button_go != null)
			settings_rect = settings_button_go.GetComponent<TheRectTransform>();
	
		if(exit_button_go != null)
			exit_rect = exit_button_go.GetComponent<TheRectTransform>();
		
		if(explanation_text_go != null)
		{
			explanation_text = explanation_text_go.GetComponent<TheText>();
			
			if(explanation_text != null)
				explanation_text.Text = "";
		}

		if(loading_text_go != null)
			loading_text_go.SetActive(false);

		if(side_selection_continue_go != null)
			continue_rect = side_selection_continue_go.GetComponent<TheRectTransform>();

		menu_audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();

		menu_audio_source.Play("Play_Menu_song");
	}
	
	void Update ()
	{
		// Campaign button
		if(campaign_rect != null)
		{
			if(campaign_rect.OnClickUp)
			{
				if(loading_text_go != null)
					loading_text_go.SetActive(true);
				
				menu_audio_source.Play("Play_click");
				menu_audio_source.Stop("Play_Menu_song");
				
				if(menu_go != null)
					menu_go.SetActive(false);
				
				if(side_selection_go != null)
					side_selection_go.SetActive(true);

				return;
			}

			if(campaign_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "Play the campaign";
			}
			
			if(campaign_rect.OnMouseEnter)
				menu_audio_source.Play("Play_hover");
			
		}
		
		// Settings button
		if(settings_rect != null)
		{
			if(settings_rect.OnClickUp)
			{
				TheConsole.Log("Settings");
				menu_audio_source.Play("Play_click");
			}

			if(settings_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "Change the game settings";
			}

			if(settings_rect.OnMouseEnter)
				menu_audio_source.Play("Play_hover");
			
		}
		
		// Exit button
		if(exit_rect != null)
		{
			if(exit_rect.OnClickUp)
			{
				menu_audio_source.Play("Play_click");
				TheApplication.Quit();
				return;
			}

			if(exit_rect.OnMouseOver)
			{
				if(explanation_text != null)
				{
					explanation_text.Text = "Exit the game";
					return;
				}
			}

			if(exit_rect.OnMouseEnter)
				menu_audio_source.Play("Play_hover");
		}
		
		// Explanation text
		if(exit_rect != null && campaign_rect != null && settings_rect != null)
		{
			if(!campaign_rect.OnMouseOver && !settings_rect.OnMouseOver && !exit_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "";
			}
		}

		// Continue button
		if(continue_rect != null)
		{
			if(continue_rect.OnMouseEnter)
				menu_audio_source.Play("Play_hover");

			if(continue_rect.OnClickUp)
			{
				menu_audio_source.Play("Play_click");
				TheApplication.LoadScene("VerticalSlice_2_TrueStory");
			}
		}

		if(pressed_sound)
		{
			menu_audio_source.Play("Play_click");
			pressed_sound = false;
		}
	}
}