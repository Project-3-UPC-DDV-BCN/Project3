using TheEngine;
using TheEngine.TheConsole;

public class Slave1Movement {

	TheTransform trans;
	public int controller_sensibility = 2500;
	public int trigger_sensibility = 0;
	public float roll_rotate_speed = 45.0f;
	public float pitch_rotate_speed = 40.0f;
	public float yaw_rotate_speed = 25.0f;
	public float max_vel = 50.0f;
	public float acceleration = 10.0f;
	public float slow_acceleration = 50.0f;
	public float slow_vel_percent = 0.02f;
	public float rotate_rumble_strength = 0.05f;
	public float accel_max_rumble_strength = 0.3f;
	public float boost_rumble_strength = 0.3f;
	public int rotate_rumble_ms = 5;
	public int accel_rumble_ms = 3;
	public int boost_rumble_ms = 3;

	private float curr_vel = 0.0f;
	private float vel_percent = 0.02f; //from 1.0f to 0.02f;
	private float curr_max_vel = 10.0f;
	private float curr_accel;
	bool slowing = false;
	public bool invert_axis = false;
	
	private bool boosting = false;
	public float boost_extra_vel = 25.0f;
	public float boost_accel_multiplier = 1.5f;
	private float boost_timer;
	public float boost_time = 1.5f;
	public float boost_cd_time = 5.0f;
	private float boost_cd_timer;
	public float slow_time = 1.0f;
	public float slow_regen = 0.1f;
	private float slow_timer;

	//HP and Shields
	public float total_hp = 99.0f;
	float curr_total_hp;
	float body_hp;
	float wings_hp;
	float engine_hp;
	public float shield_hp = 25.0f;
	float curr_shield_hp;
	public float shield_regen = 10.0f;
    public float shield_regen_time = 10.0f;
    float shield_regen_energy;
    float shield_regen_timer;

	//Energy management
	public int max_energy = 6;
	public int shield_energy = 2;
	public int weapon_energy = 2;
	public int engine_energy = 2;
	public int max_energy_on_system = 4;
	
	public TheGameObject weapons;
	public TheGameObject shields;
	public TheGameObject energy;
	public TheGameObject speed;
    public TheGameObject shield_hp_go;
    public TheGameObject hp;

    TheProgressBar weapons_bar = null;
	TheProgressBar shields_bar = null;
	TheProgressBar energy_bar = null;
	TheProgressBar speed_bar = null;
    TheProgressBar shield_hp_bar = null;
    TheProgressBar hp_bar = null;

    //audio
    public TheGameObject audio_emiter;
	
	TheAudioSource audio_source;
	
	void Start () 
	{
		trans = TheGameObject.Self.GetComponent<TheTransform>();

		if(shield_energy+weapon_energy+engine_energy > max_energy)
			TheConsole.Warning("Energy is not properly set up!");
		
		weapons_bar = weapons.GetComponent<TheProgressBar>();
		shields_bar = shields.GetComponent<TheProgressBar>();
		energy_bar = energy.GetComponent<TheProgressBar>();
		speed_bar = speed.GetComponent<TheProgressBar>();
        shield_hp_bar = shield_hp_go.GetComponent<TheProgressBar>();
        hp_bar = hp.GetComponent<TheProgressBar>();

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
		curr_accel = acceleration;

		//HP and shield
		body_hp = wings_hp = engine_hp = total_hp/3.0f;
		curr_total_hp = total_hp;
		curr_shield_hp = shield_hp;
        shield_regen_energy = shield_regen;

		audio_source = audio_emiter.GetComponent<TheAudioSource>();
		audio_source.Play("Play_Engine");
		audio_source.SetMyRTPCvalue("Speed",vel_percent);
	}
	
	void Update () 
	{
		Movement();
		EnergyManagement();
		SetValuesWithEnergy();
        RegenShield();

        //set ui bars
		weapons_bar.PercentageProgress = (100.0f / 8.0f) * weapon_energy;
		shields_bar.PercentageProgress = (100.0f / 8.0f) * shield_energy;
		energy_bar.PercentageProgress = (100.0f / 8.0f) * engine_energy;
        shield_hp_bar.PercentageProgress = (curr_shield_hp / shield_hp) * 100.0f;
        hp_bar.PercentageProgress = (curr_total_hp / total_hp) * 100.0f;

		speed_bar.PercentageProgress = (curr_vel/((1.5f * max_vel) + boost_extra_vel))  * 100;
	}

	void SetValuesWithEnergy()
	{
		curr_max_vel = 0.5f*max_vel + ((12.5f*(float)engine_energy)/100)*max_vel;
        shield_regen_energy = 0.5f*shield_regen + ((12.5f * (float)shield_energy) / 100) * shield_regen;
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

		if(TheInput.IsKeyDown("F1"))
		{
			DamageBody(10.0f);
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
		
		if(TheInput.IsKeyDown("B") && boost_cd_timer <= 0.0f)
		{
			boosting = true;
			boost_timer = boost_time;
			boost_cd_timer = 0.1f; //dont allow to boost continously
		}

		slowing = false;
		if(TheInput.GetControllerButton(0,"CONTROLLER_X") == 2)
		{
			slow_timer+=TheTime.DeltaTime;

			if(slow_timer <= slow_time)
			{
				vel_percent = slow_vel_percent;
				slowing = true;
			}
			
		}

		if(TheInput.GetControllerButton(0,"CONTROLLER_X") == 0)
		{
			if(slow_timer > 0.0f)
			{
				slow_timer-= slow_regen*TheTime.DeltaTime;
				TheConsole.Log(slow_regen*TheTime.DeltaTime);
			}
		}

		float target_vel = vel_percent*curr_max_vel;
		audio_source.SetMyRTPCvalue("Speed",(curr_vel/curr_max_vel)*100);
		
		if(boosting)
		{
			TheConsole.Log("boooooost");
			target_vel = curr_max_vel+boost_extra_vel;
			curr_accel = acceleration*boost_accel_multiplier;
			TheInput.RumbleController(0,boost_rumble_strength,boost_rumble_ms);
			if(curr_vel >= target_vel && boost_timer <= 0.0f)
			{
				boosting = false;
				curr_accel = acceleration;
				TheConsole.Log("STOOOOP! boooooost");
				boost_cd_timer = boost_cd_time;
				TheConsole.Log(boost_cd_timer);
			}

			if(curr_vel>= target_vel)
				boost_timer -= TheTime.DeltaTime;
		}
		else
		{
			if(engine_energy > 4)
			{
				boost_cd_timer -= TheTime.DeltaTime*(1+(0.5f/4)*(engine_energy-4));
			}
			else if(engine_energy < 4)
			{
				boost_cd_timer -= TheTime.DeltaTime*(1-(0.75f/4)*(engine_energy));
			}
			else
				boost_cd_timer -= TheTime.DeltaTime;
			
			TheConsole.Log(boost_cd_timer);
		
		}

		if(curr_vel < target_vel) 
		{
			curr_vel += curr_accel*TheTime.DeltaTime;
			float rumble = accel_max_rumble_strength - (curr_vel/target_vel)*accel_max_rumble_strength;
			TheInput.RumbleController(0,rumble,accel_rumble_ms);
		}
		else if(curr_vel > target_vel)
		{
			if(!slowing)
				curr_vel-=curr_accel*TheTime.DeltaTime;
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

    void DamageWings(float damage)
    {
        if (shield_hp > 0)
        {
            curr_shield_hp -= damage;
            shield_regen_timer = shield_regen_time;
        }
        else
        {
            wings_hp -= damage;
            if (wings_hp <= 0.0f)
            {
                //lose
            }
        }
    }

    void DamageBody(float damage)
    {
        if (shield_hp > 0)
        {
            curr_shield_hp -= damage;
            shield_regen_timer = shield_regen_time;
        }
        else
        {
            body_hp -= damage;
            if (wings_hp <= 0.0f)
            {
                //lose
            }
        }
    }

    void DamageEngine(float damage)
    {
        if (shield_hp > 0)
        {
            curr_shield_hp -= damage;
            shield_regen_timer = shield_regen_time;
        }
        else
        {
            engine_hp -= damage;
            if (wings_hp <= 0.0f)
            {
                //lose
            }
        }
    }

    void RegenShield()
    {
        if(shield_regen_timer<=0.0f && curr_shield_hp<shield_hp)
        {
            curr_shield_hp+=shield_regen_energy* TheTime.DeltaTime;
            if (curr_shield_hp > shield_hp)
                curr_shield_hp = shield_hp;
        }
        shield_regen_timer -= TheTime.DeltaTime;
    }

}