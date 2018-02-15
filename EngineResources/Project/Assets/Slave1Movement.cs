using TheEngine;
using TheEngine.Time;

public class Slave1Movement {

	TheTransform trans;
	public float rotate_speed = 1.0f;

	void Start () {
		trans = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () 
	{
		int rjoy_up = TheInput.GetControllerJoystickMove(0,"RIGHTJOY_UP");
		int rjoy_down = TheInput.GetControllerJoystickMove(0,"RIGHTJOY_DOWN");
		int rjoy_right = TheInput.GetControllerJoystickMove(0,"RIGHTJOY_RIGHT");
		int rjoy_left = TheInput.GetControllerJoystickMove(0,"RIGHTJOY_LEFT");
		
		int ljoy_up = TheInput.GetControllerJoystickMove(0,"LEFTJOY_UP");
		int ljoy_down = TheInput.GetControllerJoystickMove(0,"LEFTJOY_DOWN");
		int ljoy_right = TheInput.GetControllerJoystickMove(0,"LEFTJOY_RIGHT");
		int ljoy_left = TheInput.GetControllerJoystickMove(0,"LEFTJOY_LEFT");
		
		if(rjoy_up > 0)
		{
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.x += rotate_speed*(rjoy_up/32767)*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}
	}
}