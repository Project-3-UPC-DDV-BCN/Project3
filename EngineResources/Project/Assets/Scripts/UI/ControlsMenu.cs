using TheEngine;
using TheEngine.TheConsole;

public class ControlsMenu {
	
	// Menu
	public TheGameObject background;
	public TheGameObject controls_panel;
	public TheGameObject ship_controls_panel;
	
	void Update ()
	{
		if(TheInput.GetControllerButton(0, "CONTROLLER_RIGHT_ARROW") == 1 || TheInput.IsKeyDown("RIGHT_ARROW"))
		{
			ship_controls_panel.SetActive(true);
			controls_panel.SetActive(false);
		}
		
		if(TheInput.GetControllerButton(0, "CONTROLLER_LEFT_ARROW") == 1 || TheInput.IsKeyDown("LEFT_ARROW"))
		{
			controls_panel.SetActive(true);
			ship_controls_panel.SetActive(false);
		}
		
		if(TheInput.GetControllerButton(0, "CONTROLLER_START") == 1 || TheInput.IsKeyDown("ESC"))
		{
			ship_controls_panel.SetActive(false);
			controls_panel.SetActive(false);
			background.SetActive(false);
		}
	}
}