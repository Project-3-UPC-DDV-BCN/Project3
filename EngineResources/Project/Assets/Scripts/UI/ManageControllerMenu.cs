using TheEngine;
using TheEngine.TheConsole; 

public class ManageControllerMenu 
{
	
	TheCanvas canvas; 

	void Start () 
	{
		canvas = TheGameObject.Self.GetComponent<TheCanvas>(0); 
	}
	
	void Update () 
	{
		if(TheInput.IsKeyDown("UP_ARROW"))
		{
			canvas.ControllerIDUp();
			TheConsole.Log("UP"); 
		}


		if(TheInput.IsKeyDown("DOWN_ARROW"))
		{
			canvas.ControllerIDDown();
			TheConsole.Log("down"); 
		}
	}
}