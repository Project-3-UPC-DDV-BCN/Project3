using TheEngine;
using TheEngine.TheConsole; 

public class MenuController {

	TheCanvas canvas; 
	TheAudioSource main_menu_audiosource; 

	void Start ()
	{
		canvas = TheGameObject.Self.GetComponent<TheCanvas>(0); 
		main_menu_audiosource = TheGameObject.Find("MainMenuCanvas(2)").GetComponent<TheAudioSource>(); 
	}
	
	void Update ()
	{
		if(TheInput.IsKeyDown("UP_ARROW"))
		{
			canvas.ControllerIDDown(); 
			main_menu_audiosource.Play("Play_hover"); 
		}

		if(TheInput.IsKeyDown("DOWN_ARROW"))
		{
			canvas.ControllerIDUp();
			main_menu_audiosource.Play("Play_hover");
		}

		if(TheInput.IsKeyDown("RIGHT_ARROW"))
		{
			canvas.PressButton(); 
			main_menu_audiosource.Play("Play_click");
			TheApplication.LoadScene("Alpha1 - Level1Scene");
		}
	}
}