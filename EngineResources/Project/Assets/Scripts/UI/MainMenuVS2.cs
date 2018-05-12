using TheEngine;
using TheEngine.TheConsole;

public class MainMenuVS2 
{
	public TheGameObject menu_go;
	public TheCanvas menu_canvas; 
	public TheGameObject side_selection_go;
	
	// Menu
	public TheGameObject campaign_button_go;
	public TheGameObject training_button_go;
	public TheGameObject settings_button_go;
	public TheGameObject exit_button_go;
	public TheGameObject explanation_text_go;

	TheRectTransform campaign_rect = null;
	TheRectTransform training_rect = null;
	TheRectTransform settings_rect = null;
	TheRectTransform exit_rect = null;
	TheText	explanation_text = null;	
	
	// Audio
	TheAudioSource menu_audio_source = null;

	bool pressed_sound = false;
	bool over_sound = false;
	bool mouse_moved = false; 

	void Start () 
	{
		// Menu
		menu_canvas = menu_go.GetComponent<TheCanvas>(); 

		if(menu_go != null)
			menu_go.SetActive(true);
	
		if(side_selection_go != null)
			side_selection_go.SetActive(false);
	
		if(campaign_button_go != null)
			campaign_rect = campaign_button_go.GetComponent<TheRectTransform>();

		if(training_button_go != null)
			training_rect = training_button_go.GetComponent<TheRectTransform>();
		
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
		

		// Audio
		menu_audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();
		if(menu_audio_source != null)
			menu_audio_source.Play("Play_Menu_song");

		menu_canvas.EnableCurrent();
		
	}
	
	void Update ()
	{
		//Controller Managing
		int id_pressed = -1; 
		if(TheInput.GetControllerButton(0, "CONTROLLER_UP_ARROW") == 1)
		{
			menu_canvas.ControllerIDDown();
			menu_canvas.EnableCurrent();
			over_sound = true; 
		}

		if(TheInput.GetControllerButton(0, "CONTROLLER_DOWN_ARROW") == 1)
		{			
			menu_canvas.ControllerIDUp();
			menu_canvas.EnableCurrent();
			over_sound = true; 
		}

		if(TheInput.GetControllerButton(0,"CONTROLLER_A") == 1)
		{
			menu_canvas.EnableCurrent();
			menu_canvas.PressButton(); 
			id_pressed = menu_canvas.GetCurrentID(); 
			pressed_sound = true; 				
		}

		if(TheInput.GetMouseXMotion() != 0.0 || TheInput.GetMouseYMotion() != 0.0)
		{
			menu_canvas.DisableCurrent();
			mouse_moved = true; 
		}


		// Campaign button
		if(campaign_rect != null)
		{
			if(campaign_rect.OnClickUp || id_pressed == 0)
			{					
				if(menu_go != null)
					menu_go.SetActive(false);

				if(menu_audio_source != null)	
					menu_audio_source.Stop("Play_Menu_song");
				
				TheApplication.LoadScene("Alpha1 - Level1Scene");
			}

			if(campaign_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "Play the campaign";
			}
			
			if(campaign_rect.OnMouseEnter)
				over_sound = true;
		}

		// Training button
		if(training_rect != null)
		{
			if(training_rect.OnClickUp || id_pressed == 1)
			{				
				if(menu_audio_source != null)	
					menu_audio_source.Stop("Play_Menu_song");

				TheApplication.LoadScene("Alpha1 - TrainingScene");
			}

			if(training_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "Improve with the training mode";
			}
			
			if(training_rect.OnMouseEnter)
				over_sound = true;
		}
		
		// Settings button
		if(settings_rect != null)
		{
			if(settings_rect.OnClickUp || id_pressed == 2)
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
			if(exit_rect.OnClickUp || id_pressed == 3)
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
		if(exit_rect != null && campaign_rect != null && settings_rect != null && training_rect != null)
		{
			if(!campaign_rect.OnMouseOver && !settings_rect.OnMouseOver && !exit_rect.OnMouseOver && !training_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "";
			}
		}

		if(pressed_sound)
		{
			if(menu_audio_source != null)
				menu_audio_source.Play("Play_click");
			pressed_sound = false;
		}

		if(over_sound)
		{
			if(menu_audio_source != null)
				menu_audio_source.Play("Play_hover");
			over_sound = false;
		}
		
	}
}