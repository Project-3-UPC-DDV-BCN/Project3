using TheEngine;
using TheEngine.TheConsole;

public class MainMenuVS2 
{
	public TheGameObject menu_go;
	public TheGameObject side_selection_go;
	
	// Menu
	public TheGameObject campaign_button_go;
	public TheGameObject settings_button_go;
	public TheGameObject exit_button_go;
	public TheGameObject explanation_text_go;

	TheRectTransform campaign_rect = null;
	TheRectTransform settings_rect = null;
	TheRectTransform exit_rect = null;
	TheText	explanation_text = null;	

	// Side Selection
	public TheGameObject loading_text_go;
	public TheGameObject side_selection_continue_go;
	public TheGameObject rebels_idle_image_go;
	public TheGameObject rebels_selected_image_go;
	public TheGameObject empire_idle_image_go;
	public TheGameObject empire_selected_image_go;
	public TheGameObject side_selection_back_go;

	TheRectTransform continue_rect = null;
	TheRectTransform rebels_idle_rect = null;
	TheRectTransform empire_idle_rect = null;
	TheRectTransform side_selection_back_rect = null;

	string faction = "no_faction";
	
	// Audio
	TheAudioSource menu_audio_source = null;

	bool pressed_sound = false;
	bool over_sound = false;

	void Start () 
	{
		// Menu
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
		
		// Side Selection
		if(loading_text_go != null)
			loading_text_go.SetActive(false);

		if(side_selection_continue_go != null)
			continue_rect = side_selection_continue_go.GetComponent<TheRectTransform>();

		if(rebels_idle_image_go != null)
			rebels_idle_rect = rebels_idle_image_go.GetComponent<TheRectTransform>();

		if(empire_idle_image_go != null)
			empire_idle_rect = empire_idle_image_go.GetComponent<TheRectTransform>();

		if(rebels_selected_image_go != null)
			rebels_selected_image_go.SetActive(false);

		if(empire_selected_image_go != null)
			empire_selected_image_go.SetActive(false);

		if(side_selection_continue_go != null)
			side_selection_continue_go.SetActive(false);

		if(side_selection_back_go != null)
			side_selection_back_rect = side_selection_back_go.GetComponent<TheRectTransform>();

		// Audio
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
				if(menu_go != null)
					menu_go.SetActive(false);
				
				if(side_selection_go != null)
					side_selection_go.SetActive(true);
			}

			if(campaign_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "Play the campaign";
			}
			
			if(campaign_rect.OnMouseEnter)
				over_sound = true;
		}
		
		// Settings button
		if(settings_rect != null)
		{
			if(settings_rect.OnClickUp)
			{
				TheConsole.Log("Settings");
				pressed_sound = true;
			}

			if(settings_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "Change the game settings";
			}

			if(settings_rect.OnMouseEnter)
				over_sound = true;
			
		}
		
		// Exit button
		if(exit_rect != null)
		{
			if(exit_rect.OnClickUp)
			{
				pressed_sound = true;
				TheApplication.Quit();
			}

			if(exit_rect.OnMouseOver)
			{
				if(explanation_text != null)
				{
					explanation_text.Text = "Exit the game";
				}
			}

			if(exit_rect.OnMouseEnter)
				over_sound = true;
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
				over_sound = true;

			if(continue_rect.OnClickUp && faction != "no_faction")
			{
				pressed_sound = true;

				menu_audio_source.Stop("Play_Menu_song");

				if(loading_text_go != null)
					loading_text_go.SetActive(true);

				TheApplication.LoadScene("VerticalSlice_2_TrueStory");
			}
		}

		// Back sideselection rect
		if(side_selection_back_rect != null)
		{
			if(side_selection_back_rect.OnMouseEnter)
				over_sound = true;

			if(side_selection_back_rect.OnClickUp)
			{
				pressed_sound = true;
				faction = "";

				if(side_selection_go != null)
					side_selection_go.SetActive(false);

				if(menu_go != null)
					menu_go.SetActive(true);
			}
		}

		// Rebels rect
		if(rebels_idle_rect != null)
		{
			if(rebels_idle_rect.OnClickUp)
			{
				pressed_sound = true;
				faction = "rebels";

				if(rebels_selected_image_go != null)
					rebels_selected_image_go.SetActive(true);

				if(empire_selected_image_go != null)
					empire_selected_image_go.SetActive(false);

				if(side_selection_continue_go != null)
					side_selection_continue_go.SetActive(true);
			}
		}

		// Empire rect
		if(empire_idle_rect != null)
		{
			if(empire_idle_rect.OnClickUp)
			{
				pressed_sound = true;
				faction = "empire";

				if(empire_selected_image_go != null)
					empire_selected_image_go.SetActive(true);

				if(rebels_selected_image_go != null)
					rebels_selected_image_go.SetActive(false);

				if(side_selection_continue_go != null)
					side_selection_continue_go.SetActive(true);
			}
		}

		if(pressed_sound)
		{
			menu_audio_source.Play("Play_click");
			pressed_sound = false;
		}

		if(over_sound)
		{
			menu_audio_source.Play("Play_hover");
			over_sound = false;
		}
	}
}