using TheEngine;
using TheEngine.TheConsole;

public class test_controller {

	void Start () {
		
	}
	
	void Update () {
		if(TheInput.GetControllerButton(0,"CONTROLLER_A") == 1)
		{
			TheConsole.Error("Pressed A");
		}

	}
}