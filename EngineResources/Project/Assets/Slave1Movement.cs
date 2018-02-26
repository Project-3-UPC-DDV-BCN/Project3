using TheEngine;
using TheEngine.TheConsole;

public class Slave1Movement {

	TheTransform trans;
	public int controller_sensibility = 100;
	public int trigger_sensibility = 0;
	public float rotate_speed = 1.0f;
	public float max_vel = 10.0f;
	public float acceleration = 1.0f;
	public float slow_acceleration = 10.0f;

	public float rotate_rumble_strength = 0.1f;
	public float accel_max_rumble_strength = 1.0f;
	public int rotate_rumble_ms = 5;
	public int accel_rumble_ms = 5;
	
	private float curr_vel = 0.0f;
	private float vel_percent = 0.02f; //from 1.0f to 0.02f;
	bool slowing = false;
	
	void Start () 
	{
		trans = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update ()
	{
		TheAudio.SetRTPvalue("Speed", curr_vel);

		//int rjoy_up = TheInput.GetControllerJoystickMove(0,"RIGHTJOY_UP");
		//int rjoy_down = TheInput.GetControllerJoystickMove(0,"RIGHTJOY_DOWN");
		//int rjoy_right = TheInput.GetControllerJoystickMove(0,"RIGHTJOY_RIGHT");
		//int rjoy_left = TheInput.GetControllerJoystickMove(0,"RIGHTJOY_LEFT");
		
		int ljoy_up = TheInput.GetControllerJoystickMove(0,"LEFTJOY_UP");
		int ljoy_down = TheInput.GetControllerJoystickMove(0,"LEFTJOY_DOWN");
		int ljoy_right = TheInput.GetControllerJoystickMove(0,"LEFTJOY_RIGHT");
		int ljoy_left = TheInput.GetControllerJoystickMove(0,"LEFTJOY_LEFT");
		
		int right_trigger = TheInput.GetControllerJoystickMove(0,"RIGHT_TRIGGER");
		//int left_trigger = TheInput.GetControllerJoystickMove(0,"LEFT_TRIGGER");
		
		if(ljoy_up > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_up - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.x -= rotate_speed*move_percentage*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(ljoy_down > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_down - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.x += rotate_speed*move_percentage*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(ljoy_right > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_right - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.y -= rotate_speed*move_percentage*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(ljoy_left > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_left - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.y += rotate_speed*move_percentage*Time.DeltaTime;
			trans.LocalRotation = new_rot;
		}
		
		vel_percent = 0.1f; //reset to min vel
		if(right_trigger > trigger_sensibility)
		{
			vel_percent = (float)(right_trigger - trigger_sensibility)/(float)(TheInput.MaxJoystickMove - trigger_sensibility);
			if(vel_percent<0.1f) vel_percent = 0.1f;
		}
		
		if(TheInput.GetControllerButton(0,"CONTROLLER_RB") == 2)
		{
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.z += rotate_speed*Time.DeltaTime;
			trans.LocalRotation = new_rot;
			TheInput.RumbleController(0,rotate_rumble_strength,rotate_rumble_ms);
		}
		
		if(TheInput.GetControllerButton(0,"CONTROLLER_LB") == 2)
		{
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.z -= rotate_speed*Time.DeltaTime;
			trans.LocalRotation = new_rot;
			TheInput.RumbleController(0,rotate_rumble_strength,rotate_rumble_ms);
		}
		
		slowing = false;
		if(TheInput.GetControllerButton(0,"CONTROLLER_X") == 2)
		{
			vel_percent = 0.02f;
			slowing = true;
		}

		float target_vel = vel_percent*max_vel;

		if(curr_vel < target_vel) 
		{
			curr_vel += acceleration*Time.DeltaTime;
			float rumble = accel_max_rumble_strength - (curr_vel/target_vel)*accel_max_rumble_strength;
			TheInput.RumbleController(0,rumble,accel_rumble_ms);
		}
		else if(curr_vel > target_vel)
		{
			if(!slowing)
				curr_vel-=acceleration*Time.DeltaTime;
			else
			{
				curr_vel-=slow_acceleration*Time.DeltaTime;
				float rumble = accel_max_rumble_strength - (target_vel/curr_vel)*accel_max_rumble_strength;
				TheInput.RumbleController(0,rumble,accel_rumble_ms);
			}
		}
		
		TheVector3 new_vel_pos = trans.LocalPosition;
		new_vel_pos += trans.ForwardDirection*curr_vel*Time.DeltaTime;
		trans.LocalPosition = new_vel_pos;
	}
}