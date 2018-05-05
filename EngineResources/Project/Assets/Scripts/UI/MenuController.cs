using TheEngine;
using TheEngine.TheConsole; 

public class MenuController {

	TheCanvas canvas; 

	void Start ()
	{
		canvas = TheGameObject.Self.GetComponent<TheCanvas>(0); 
	}
	
	void Update ()
	{
		if(TheInput.IsKeyDown("UP_ARROW"))
		{
			canvas.ControllerIDDown(); 
			TheConsole.Log("UP"); 
		}

		if(TheInput.IsKeyDown("DOWN_ARROW"))
		{
			canvas.ControllerIDUp();
			TheConsole.Log("down"); 
		}

		if(TheInput.IsKeyDown("RIGHT_ARROW"))
		{
			canvas.PressButton(); 
		}
	}
}