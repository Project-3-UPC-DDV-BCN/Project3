using TheEngine;
using TheEngine.TheConsole;

public class PauseMenu {

	public TheCanvas menu_canvas; 
	
	// Menu
	public TheGameObject continue_button_go;
	public TheGameObject controls_button_go;
	public TheGameObject exit_button_go;
	
	public TheGameObject canvas_go;
	public TheGameObject controls_menu;
	public TheGameObject background;
	public TheGameObject controls_panel;

	TheRectTransform continue_rect = null;
	TheRectTransform controls_rect = null;
	TheRectTransform exit_rect = null;	
	
	// Audio
	TheAudioSource menu_audio_source = null;
	
	public TheGameObject game_manager_go;
	TheScript game_manager = null;

	bool pressed_sound = false;
	bool over_sound = false;
	bool mouse_moved = false; 

	void Start () 
	{
		// Menu
		//menu_canvas = canvas_go.GetComponent<TheCanvas>(); 
		if(continue_button_go != null)
			continue_rect = continue_button_go.GetComponent<TheRectTransform>();

		if(controls_button_go != null)
			controls_rect = controls_button_go.GetComponent<TheRectTransform>();
	
		if(exit_button_go != null)
			exit_rect = exit_button_go.GetComponent<TheRectTransform>();
		

		// Audio
		menu_audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();

		//menu_canvas.EnableCurrent();
		
		game_manager = game_manager_go.GetScript("GameManager");
	}
	
	void Update ()
	{
		//Controller Managing
		int id_pressed = -1; 
		/*if(TheInput.GetControllerButton(0, "CONTROLLER_UP_ARROW") == 1)
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
		}*/


		// Campaign button
		if(continue_rect != null)
		{
			if(continue_rect.OnClickUp || id_pressed == 0)
			{					
				if(canvas_go != null)
				{
					canvas_go.SetActive(false);
					object[] args = {false};
					game_manager.CallFunctionArgs("PauseGame", args);
				}
			}
			
			if(continue_rect.OnMouseEnter)
				over_sound = true;
		}

		// Training button
		if(controls_rect != null)
		{
			if(controls_rect.OnClickUp || id_pressed == 1)
			{				
				controls_panel.SetActive(true);
				background.SetActive(true);
				canvas_go.SetActive(false);
				TheScript controls_menu_script = controls_menu.GetScript("ControlsMenu");
				if(controls_menu_script != null)
				{
					controls_menu_script.CallFunction("ActiveMenu");
				}
			}
			
			if(controls_rect.OnMouseEnter)
				over_sound = true;
		}
		
		// Exit button
		if(exit_rect != null)
		{
			if(exit_rect.OnClickUp || id_pressed == 2)
			{
				pressed_sound = true;
				TheApplication.DrawMouse(true);
				TheApplication.LoadScene("Alpha1 - MainMenuScene");
			}

			if(exit_rect.OnMouseEnter)
				over_sound = true;
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