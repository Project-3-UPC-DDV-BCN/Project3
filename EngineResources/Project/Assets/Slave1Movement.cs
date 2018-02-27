using TheEngine;
using TheEngine.TheConsole;

public class Slave1Movement {

	TheTransform trans;
	public int controller_sensibility = 100;
	public int trigger_sensibility = 0;
	public float roll_rotate_speed = 1.0f;
	public float pitch_rotate_speed = 10.0f;
	public float yaw_rotate_speed = 10.0f;
	public float max_vel = 10.0f;
	public float acceleration = 1.0f;
	public float slow_acceleration = 10.0f;
	public float slow_vel_percent = 0.02f;
	public float rotate_rumble_strength = 0.1f;
	public float accel_max_rumble_strength = 1.0f;
	public int rotate_rumble_ms = 5;
	public int accel_rumble_ms = 5;
	
	private float curr_vel = 0.0f;
	private float vel_percent = 0.02f; //from 1.0f to 0.02f;
	private float curr_max_vel = 10.0f;
	bool slowing = false;
	public bool invert_axis = false;
	
	public float slow_time = 2.0f;
	//Energy management
	public int max_energy = 6;
	public int shield_energy = 2;
	public int weapon_energy = 2;
	public int engine_energy = 2;
	public int max_energy_on_system = 4;
	
	public TheGameObject weapons;
	public TheGameObject shields;
	public TheGameObject energy;

	TheProgressBar weapons_bar = null;
	TheProgressBar shields_bar = null;
	TheProgressBar energy_bar = null;
	
	void Start () 
	{
		trans = TheGameObject.Self.GetComponent<TheTransform>();

		if(shield_energy+weapon_energy+engine_energy > max_energy)
			TheConsole.Warning("Energy is not properly set up!");
		
		weapons_bar = weapons.GetComponent<TheProgressBar>();
		shields_bar = shields.GetComponent<TheProgressBar>();
		energy_bar = energy.GetComponent<TheProgressBar>();

		weapons_bar.PercentageProgress = 100;
		shields_bar.PercentageProgress = 100;
		energy_bar.PercentageProgress = 100;
		//for halves, consider a point each 2 
		max_energy *= 2;
		shield_energy *= 2;
		weapon_energy *= 2;
		engine_energy *= 2;
		max_energy_on_system *= 2;
		
		curr_max_vel = max_vel;
	}
	
	void Update () 
	{
		Movement();
		EnergyManagement();
		SetValuesWithEnergy();

		weapons_bar.PercentageProgress = (100 / 8) * weapon_energy;
		shields_bar.PercentageProgress = (100 / 8) * shield_energy;
		energy_bar.PercentageProgress = (100 / 8) * engine_energy;
	}

	void SetValuesWithEnergy()
	{
		curr_max_vel = 0.5f*max_vel + ((12.5f*(float)engine_energy)/100)*max_vel;
	}

	void EnergyManagement()
	{
		if(TheInput.IsKeyDown("UP_ARROW"))
		{
			if(shield_energy < max_energy_on_system-1)
			{
				shield_energy += 2;
				if(weapon_energy > 0 && engine_energy > 0)
				{
					weapon_energy -= 1;
					engine_energy -= 1;
				}
				else if(weapon_energy > 2)
					weapon_energy -= 2;
				else engine_energy -=2;
			}
		}
		
		if(TheInput.IsKeyDown("LEFT_ARROW"))
		{
			if(weapon_energy < max_energy_on_system-1)
			{
				weapon_energy += 2;
				if(shield_energy > 0 && engine_energy > 0)
				{
					shield_energy -= 1;
					engine_energy -= 1;
				}
				else if(shield_energy > 2)
					shield_energy -= 2;
				else engine_energy -=2;
			}
		}

		if(TheInput.IsKeyDown("RIGHT_ARROW"))
		{
			if(engine_energy < max_energy_on_system-1)
			{
				engine_energy += 2;
				if(shield_energy > 0 && weapon_energy > 0)
				{
					shield_energy -= 1;
					weapon_energy -= 1;
				}
				else if(shield_energy > 2)
					shield_energy -= 2;
				else weapon_energy -=2;
			}
		}

		if(TheInput.IsKeyDown("DOWN_ARROW"))
		{
			shield_energy = 4;
			weapon_energy = 4;
			engine_energy = 4;
		}
	}

	void Movement()
	{
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
			if(invert_axis) new_rot.x += pitch_rotate_speed*move_percentage*TheTime.DeltaTime;
			else new_rot.x -= pitch_rotate_speed*move_percentage*TheTime.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(ljoy_down > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_down - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			if(invert_axis) new_rot.x -= pitch_rotate_speed*move_percentage*TheTime.DeltaTime;
			else new_rot.x += pitch_rotate_speed*move_percentage*TheTime.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(ljoy_right > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_right - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.y -= yaw_rotate_speed*move_percentage*TheTime.DeltaTime;
			trans.LocalRotation = new_rot;
		}

		if(ljoy_left > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_left - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.y += yaw_rotate_speed*move_percentage*TheTime.DeltaTime;
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
			new_rot.z += roll_rotate_speed*TheTime.DeltaTime;
			trans.LocalRotation = new_rot;
			TheInput.RumbleController(0,rotate_rumble_strength,rotate_rumble_ms);
		}
		
		if(TheInput.GetControllerButton(0,"CONTROLLER_LB") == 2)
		{
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.z -= roll_rotate_speed*TheTime.DeltaTime;
			trans.LocalRotation = new_rot;
			TheInput.RumbleController(0,rotate_rumble_strength,rotate_rumble_ms);
		}
		
		if(TheInput.GetControllerButton(0,"CONTROLLER_X") == 1)
			//slow_timer = Time.time;

		slowing = false;
		if(TheInput.GetControllerButton(0,"CONTROLLER_X") == 2)
		{
			//TheConsole.Log(Time.time);
			//if(Time.time - slow_timer < slow_time)
			//{
				//vel_percent = slow_vel_percent;
				//slowing = true;
			//}
		}

		float target_vel = vel_percent*curr_max_vel;

		if(curr_vel < target_vel) 
		{
			curr_vel += acceleration*TheTime.DeltaTime;
			float rumble = accel_max_rumble_strength - (curr_vel/target_vel)*accel_max_rumble_strength;
			TheInput.RumbleController(0,rumble,accel_rumble_ms);
		}
		else if(curr_vel > target_vel)
		{
			if(!slowing)
				curr_vel-=acceleration*TheTime.DeltaTime;
			else
			{
				curr_vel-=slow_acceleration*TheTime.DeltaTime;
				float rumble = accel_max_rumble_strength - (target_vel/curr_vel)*accel_max_rumble_strength;
				TheInput.RumbleController(0,rumble,accel_rumble_ms);
			}
		}
		
		TheVector3 new_vel_pos = trans.LocalPosition;
		new_vel_pos += trans.ForwardDirection*curr_vel*TheTime.DeltaTime;
		trans.LocalPosition = new_vel_pos;
	}
}