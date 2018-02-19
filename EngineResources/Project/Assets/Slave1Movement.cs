using TheEngine;

public class Slave1Movement {

	TheTransform trans;
	public float rotate_speed = 1.0f;
	public float vertical_thrust = 5.0f;
	public float throttle_increment = 1.0f;
	
	public int controller_sensibility = 100;
	
	public float max_vel = 10.0f;
	private float curr_vel = 0.0f;
	

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
		
		int right_trigger = TheInput.GetControllerJoystickMove(0,"RIGHT_TRIGGER");
		int left_trigger = TheInput.GetControllerJoystickMove(0,"LEFT_TRIGGER");
		
		if(ljoy_up > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_up - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.x += rotate_speed*move_percentage*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(ljoy_down > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_down - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.x -= rotate_speed*move_percentage*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(ljoy_right > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_right - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.z += rotate_speed*move_percentage*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(ljoy_left > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_left - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.z -= rotate_speed*move_percentage*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(rjoy_up > controller_sensibility)
		{
			float move_percentage = (float)(rjoy_up - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_pos = trans.LocalPosition;
			new_pos += vertical_thrust*move_percentage*Time.DeltaTime*trans.UpDirection;
			trans.LocalPosition = new_pos;
		}

		if(rjoy_down > controller_sensibility)
		{
			float move_percentage = (float)(rjoy_down - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_pos = trans.LocalPosition;
			new_pos -= vertical_thrust*move_percentage*Time.DeltaTime*trans.UpDirection;
			trans.LocalPosition = new_pos;
		}

		if(rjoy_right > controller_sensibility)
		{
			float move_percentage = (float)(rjoy_right - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.y -= rotate_speed*move_percentage*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(rjoy_left > controller_sensibility)
		{
			float move_percentage = (float)(rjoy_left - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.y += rotate_speed*move_percentage*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}
		
		if(right_trigger > controller_sensibility)
		{
			float move_percentage = (float)(right_trigger - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			if(curr_vel < max_vel)
				curr_vel += move_percentage*throttle_increment;
		}
		
		if(left_trigger > controller_sensibility)
		{
			float move_percentage = (float)(left_trigger - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			if(curr_vel > -max_vel)
				curr_vel -= move_percentage*throttle_increment;
		}
		
		TheVector3 new_vel_pos = trans.LocalPosition;
		new_vel_pos += trans.ForwardDirection*curr_vel*Time.DeltaTime;
		trans.LocalPosition = new_vel_pos;
	}
}