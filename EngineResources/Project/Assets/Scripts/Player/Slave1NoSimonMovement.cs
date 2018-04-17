using TheEngine;
using TheEngine.TheConsole;

public class Slave1NoSimonMovement {

	TheTransform trans;

    public bool casual_mode = true;

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

    public float vertical_thrust = 5.0f;
    public float throttle_increment = 10.0f;

	private float curr_vel = 1.0f;
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
	
	//Camera Movement
	public float max_camera_rot = 10.0f;
	public float max_camera_displacement = 0.1f;
    public float camera_rot_step = 1.0f;
    public TheGameObject camera_go;
    TheVector3 original_cam_rot;
    TheVector3 original_cam_pos;
    TheVector3 cam_rot;
    TheVector3 cam_pos;

    //Repair Puzzle
    int ring_exterior_pos = 0;
    int ring_center_pos = 0;
    int ring_interior_pos = 0;
    int num_ring_pos = 4;
    int repair_ring = 0;
    int num_rings = 3;
    int repair_part = 0;
    int ship_parts = 3;
    bool repair_mode = false;
    public int rand_rotate_pos = 50;
    public float repair_hp = 5.0f;
    public TheGameObject inner_ring;
    public TheGameObject center_ring;
    public TheGameObject exterior_ring;
	TheRectTransform inner_ring_trans;
    TheRectTransform center_ring_trans;
    TheRectTransform exterior_ring_trans;
	
	public TheGameObject selected_inner_ring;
	public TheGameObject selected_center_ring;
	public TheGameObject selected_exterior_ring;
	TheRectTransform selected_inner_ring_trans;
    TheRectTransform selected_center_ring_trans;
    TheRectTransform selected_exterior_ring_trans;
	
    public TheGameObject body_part;
    public TheGameObject wings_part;
    public TheGameObject engine_part;

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
		
		if(weapons != null)
			weapons_bar = weapons.GetComponent<TheProgressBar>();
	
		if(shields != null)
			shields_bar = shields.GetComponent<TheProgressBar>();

		if(energy != null)
			energy_bar = energy.GetComponent<TheProgressBar>();

		if(speed != null)
			speed_bar = speed.GetComponent<TheProgressBar>();

		if(shield_hp_go != null)
        	shield_hp_bar = shield_hp_go.GetComponent<TheProgressBar>();

		if(hp != null)
       		hp_bar = hp.GetComponent<TheProgressBar>();

		if(weapons_bar != null)
       		weapons_bar.PercentageProgress = 100;

		if(shields_bar != null)
			shields_bar.PercentageProgress = 100;

		if(energy_bar != null)
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

        //Camera
		if(camera_go != null)
		{
        	original_cam_pos = camera_go.GetComponent<TheTransform>().LocalPosition;
        	original_cam_rot = camera_go.GetComponent<TheTransform>().LocalRotation;
		}
        cam_pos = new TheVector3(0, 0, 0);
        cam_rot = new TheVector3(0, 0, 0);

        //Repair Puzzle
		if(inner_ring != null)
		{
       		inner_ring_trans = inner_ring.GetComponent<TheRectTransform>();
			inner_ring.SetActive(false);
		}
		
		if(center_ring != null)
		{
       		center_ring_trans = center_ring.GetComponent<TheRectTransform>();
       		center_ring.SetActive(false);
		}

		if(exterior_ring != null)
		{
       		exterior_ring_trans = exterior_ring.GetComponent<TheRectTransform>();
       	 	exterior_ring.SetActive(false);
		}
		
		if(selected_inner_ring != null)
		{
			selected_inner_ring_trans = selected_inner_ring.GetComponent<TheRectTransform>();
			selected_inner_ring.SetActive(false);
		}

		if(selected_center_ring != null)
		{
       		selected_center_ring_trans = selected_center_ring.GetComponent<TheRectTransform>();
			selected_center_ring.SetActive(false);
		}

		if(selected_exterior_ring != null)
		{
       		selected_exterior_ring_trans = selected_exterior_ring.GetComponent<TheRectTransform>();
			selected_exterior_ring.SetActive(false);
		}
		
		if(audio_emiter != null)
		{
       		audio_source = audio_emiter.GetComponent<TheAudioSource>();

			if(audio_source != null)
			{
				audio_source.Play("Play_Engine");
				audio_source.SetMyRTPCvalue("Speed",vel_percent);
			}
		}
	}
	
	void Update () 
	{

        NoCasualMovement();

		EnergyManagement();
		SetValuesWithEnergy();
        RegenShield();
		RepairPuzzle();

        //set ui bars
		if(weapons_bar != null)
			weapons_bar.PercentageProgress = (100.0f / 8.0f) * weapon_energy;
		
		if(shields_bar != null)
			shields_bar.PercentageProgress = (100.0f / 8.0f) * shield_energy;

		if(energy_bar != null)
			energy_bar.PercentageProgress = (100.0f / 8.0f) * engine_energy;
		
		if(shield_hp_bar != null)
        	shield_hp_bar.PercentageProgress = (curr_shield_hp / shield_hp) * 100.0f;

		if(hp_bar != null)
        	hp_bar.PercentageProgress = (curr_total_hp / total_hp) * 100.0f;

		if(speed_bar != null)
			speed_bar.PercentageProgress = (curr_vel/((1.5f * max_vel) + boost_extra_vel))  * 100;

        curr_total_hp = wings_hp + body_hp + engine_hp;
	}

	void SetValuesWithEnergy()
	{
		curr_max_vel = 0.5f * max_vel + ((12.5f*(float)engine_energy)/100) * max_vel;
        shield_regen_energy = 0.5f * shield_regen + ((12.5f * (float)shield_energy) / 100) * shield_regen;
    }

	void EnergyManagement()
	{
		if(TheInput.IsKeyDown("UP_ARROW"))
		{
			audio_source.Stop("Play_droid_speed_up");
			audio_source.Play("Play_droid_speed_up");
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
				
				else 
					engine_energy -=2;
			}
		}
		
		if(TheInput.IsKeyDown("LEFT_ARROW"))
		{
			audio_source.Stop("Play_Shield_up");
			audio_source.Play("Play_Shield_up");
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

				else 
					engine_energy -=2;
			}
		}

		if(TheInput.IsKeyDown("RIGHT_ARROW"))
		{
			audio_source.Stop("Play_Potency_up");
			audio_source.Play("Play_Potency_up");
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
				else 
					weapon_energy -=2;
			}
		}

		if(TheInput.IsKeyDown("DOWN_ARROW"))
		{
			audio_source.Stop("Play_droid_speed_down");
			audio_source.Play("Play_droid_speed_down");
			shield_energy = 4;
			weapon_energy = 4;
			engine_energy = 4;
		}

		if(TheInput.IsKeyDown("F1"))
		{
			DamageSlaveOne(10.0f);
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

            if (invert_axis)
            {
                new_rot.x += pitch_rotate_speed * move_percentage * TheTime.DeltaTime;

                if (cam_rot.x > -max_camera_rot * move_percentage && cam_rot.x <= 0.0f)
                {
                    cam_rot.x -= camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x < -max_camera_rot * move_percentage)
                    {
                        cam_rot.x = -max_camera_rot * move_percentage;
                    }
                }
            }
            else
            {
                new_rot.x -= pitch_rotate_speed * move_percentage * TheTime.DeltaTime;
                if (cam_rot.x < max_camera_rot * move_percentage && cam_rot.x >= 0.0f)
                {
                    cam_rot.x += camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x > max_camera_rot * move_percentage)
                        cam_rot.x = max_camera_rot * move_percentage;
                }
            }
            trans.SetIncrementalRotation(new_rot);

		}
        else
        {
            if (invert_axis)
            {
                if (cam_rot.x < 0.0f)
                {
                    cam_rot.x += camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x > 0.0f)
                        cam_rot.x = 0.0f;
                }
            }
            else
            {
                if (cam_rot.x > 0.0f)
                {
                    cam_rot.x -= camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x < 0.0f)
                        cam_rot.x = 0.0f;
                }
            }
        }


        if (ljoy_down > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_down - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
			TheVector3 new_rot = trans.LocalRotation;
            if (invert_axis)
            {
                new_rot.x -= pitch_rotate_speed * move_percentage * TheTime.DeltaTime;
                if (cam_rot.x < max_camera_rot * move_percentage && cam_rot.x >= 0.0f)
                {
                    cam_rot.x += camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x > max_camera_rot * move_percentage)
                        cam_rot.x = max_camera_rot * move_percentage;
                }
            }
            else
            {
                new_rot.x += pitch_rotate_speed * move_percentage * TheTime.DeltaTime;
                if (cam_rot.x > -max_camera_rot * move_percentage && cam_rot.x <= 0.0f)
                {
                    cam_rot.x -= camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x < -max_camera_rot * move_percentage)
                    {
                        cam_rot.x = -max_camera_rot * move_percentage;
                    }
                }
            }
            trans.SetIncrementalRotation(new_rot);
        }
        else
        {
            if (invert_axis)
            {
                if (cam_rot.x > 0.0f)
                {
                    cam_rot.x -= camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x < 0.0f)
                        cam_rot.x = 0.0f;
                }
            }
            else
            {
                if (cam_rot.x < 0.0f)
                {
                    cam_rot.x += camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x > 0.0f)
                        cam_rot.x = 0.0f;
                }
            }
        }

        if (ljoy_right > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_right - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.y -= yaw_rotate_speed * move_percentage * TheTime.DeltaTime;
            trans.SetIncrementalRotation(new_rot);
            if (cam_rot.y < max_camera_rot * move_percentage && cam_rot.y >= 0.0f)
            {
                cam_rot.y += camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.y > max_camera_rot * move_percentage)
                    cam_rot.y = max_camera_rot * move_percentage;
            }
        }
        else
        {
            if (cam_rot.y > 0.0f)
            {
                cam_rot.y -= camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.y < 0.0f)
                    cam_rot.y = 0.0f;
            }
        }

        if (ljoy_left > controller_sensibility)
		{
			float move_percentage = (float)(ljoy_left - controller_sensibility)/(float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.y += yaw_rotate_speed * move_percentage * TheTime.DeltaTime;
            trans.SetIncrementalRotation(new_rot);
            if (cam_rot.y > -max_camera_rot * move_percentage && cam_rot.y <= 0.0f)
            {
                cam_rot.y -= camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.y < -max_camera_rot * move_percentage)
                    cam_rot.y = -max_camera_rot * move_percentage;
            }
        }
        else
        {
            if (cam_rot.y < 0.0f)
            {
                cam_rot.y += camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.y > 0.0f)
                    cam_rot.y = 0.0f;
            }
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
            trans.SetIncrementalRotation(new_rot);
            TheInput.RumbleController(0,rotate_rumble_strength,rotate_rumble_ms);
            if (cam_rot.z > -max_camera_rot && cam_rot.z <= 0.0f)
            {
                cam_rot.z -= camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.z < -max_camera_rot)
                    cam_rot.z = -max_camera_rot;
            }
		}
        else
        {
            if(cam_rot.z<0.0f)
            {
                cam_rot.z += camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.z > 0.0f)
                    cam_rot.z = 0.0f;
            }
        }
		
		if(TheInput.GetControllerButton(0,"CONTROLLER_LB") == 2)
		{
			TheVector3 new_rot = trans.LocalRotation;
			new_rot.z -= roll_rotate_speed*TheTime.DeltaTime;
            trans.SetIncrementalRotation(new_rot);
            TheInput.RumbleController(0,rotate_rumble_strength,rotate_rumble_ms);
            if (cam_rot.z < max_camera_rot && cam_rot.z >= 0.0f)
            {
                cam_rot.z += camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.z > max_camera_rot)
                    cam_rot.z = max_camera_rot;
            }
        }
        else
        {
            if (cam_rot.z > 0.0f)
            {
                cam_rot.z -= camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.z < 0.0f)
                    cam_rot.z = 0.0f;
            }
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
			}
		}

		float target_vel = vel_percent * curr_max_vel;

		if(audio_source != null)
			audio_source.SetMyRTPCvalue("Speed",(curr_vel/((1.5f * max_vel) + boost_extra_vel))*100);
		
		if(boosting)
		{
			target_vel = curr_max_vel + boost_extra_vel;
			curr_accel = acceleration * boost_accel_multiplier;

			TheInput.RumbleController(0, boost_rumble_strength,boost_rumble_ms);

			if(curr_vel >= target_vel && boost_timer <= 0.0f)
			{
				boosting = false;
				curr_accel = acceleration;
				boost_cd_timer = boost_cd_time;
			}

			if(curr_vel>= target_vel)
				boost_timer -= TheTime.DeltaTime;
		}
		else
		{
			if(engine_energy > 4)
			{
				boost_cd_timer -= TheTime.DeltaTime*(1+(0.5f/4) * (engine_energy-4));
			}
			else if(engine_energy < 4)
			{
				boost_cd_timer -= TheTime.DeltaTime*(1-(0.75f/4) * (engine_energy));
			}
			else
				boost_cd_timer -= TheTime.DeltaTime;
		
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
		
		if(camera_go != null)
		{
        	camera_go.GetComponent<TheTransform>().LocalPosition = original_cam_pos + cam_pos;
        	camera_go.GetComponent<TheTransform>().LocalRotation = original_cam_rot + cam_rot;
		}
    }

    void NoCasualMovement()
    {
        int rjoy_up = TheInput.GetControllerJoystickMove(0, "RIGHTJOY_UP");
        int rjoy_down = TheInput.GetControllerJoystickMove(0, "RIGHTJOY_DOWN");
        int rjoy_right = TheInput.GetControllerJoystickMove(0, "RIGHTJOY_RIGHT");
        int rjoy_left = TheInput.GetControllerJoystickMove(0, "RIGHTJOY_LEFT");

        int ljoy_up = TheInput.GetControllerJoystickMove(0, "LEFTJOY_UP");
        int ljoy_down = TheInput.GetControllerJoystickMove(0, "LEFTJOY_DOWN");
        int ljoy_right = TheInput.GetControllerJoystickMove(0, "LEFTJOY_RIGHT");
        int ljoy_left = TheInput.GetControllerJoystickMove(0, "LEFTJOY_LEFT");

        int right_trigger = TheInput.GetControllerJoystickMove(0, "RIGHT_TRIGGER");
        int left_trigger = TheInput.GetControllerJoystickMove(0, "LEFT_TRIGGER");

        if (ljoy_up > controller_sensibility)
        {
            float move_percentage = (float)(ljoy_up - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.x += pitch_rotate_speed * move_percentage * TheTime.DeltaTime;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.x < max_camera_rot * move_percentage && cam_rot.x >= 0.0f)
                {
                    cam_rot.x += camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x > max_camera_rot * move_percentage)
                        cam_rot.x = max_camera_rot * move_percentage;
                }
        }
		else
        {
			if (cam_rot.x > 0.0f)
                {
                    cam_rot.x -= camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x < 0.0f)
                        cam_rot.x = 0.0f;
                }
		}

        if (ljoy_down > controller_sensibility)
        {
            float move_percentage = (float)(ljoy_down - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.x -= pitch_rotate_speed * move_percentage * TheTime.DeltaTime;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.x > -max_camera_rot * move_percentage && cam_rot.x <= 0.0f)
                {
                    cam_rot.x -= camera_rot_step * TheTime.DeltaTime;
                    if (cam_rot.x < -max_camera_rot * move_percentage)
                    {
                        cam_rot.x = -max_camera_rot * move_percentage;
                    }
                }
        }
		else
        {
            
            if (cam_rot.x < 0.0f)
            {
                cam_rot.x += camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.x > 0.0f)
                    cam_rot.x = 0.0f;
            }
            
        }

        if (ljoy_right > controller_sensibility)
        {
            float move_percentage = (float)(ljoy_right - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.z += roll_rotate_speed * move_percentage * TheTime.DeltaTime;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.z > -max_camera_rot && cam_rot.z <= 0.0f)
            {
                cam_rot.z -= camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.z < -max_camera_rot)
                    cam_rot.z = -max_camera_rot;
            }
		}
        else
        {
            if(cam_rot.z<0.0f)
            {
                cam_rot.z += camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.z > 0.0f)
                    cam_rot.z = 0.0f;
            }
        }

        if (ljoy_left > controller_sensibility)
        {
            float move_percentage = (float)(ljoy_left - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.z -= roll_rotate_speed * move_percentage * TheTime.DeltaTime;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.z < max_camera_rot && cam_rot.z >= 0.0f)
            {
                cam_rot.z += camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.z > max_camera_rot)
                    cam_rot.z = max_camera_rot;
            }
        }
        else
        {
            if (cam_rot.z > 0.0f)
            {
                cam_rot.z -= camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.z < 0.0f)
                    cam_rot.z = 0.0f;
            }
        }

        if (rjoy_up > controller_sensibility)
        {
            float move_percentage = (float)(rjoy_up - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_pos = trans.LocalPosition;
            new_pos += vertical_thrust * move_percentage * TheTime.DeltaTime * trans.UpDirection;
            trans.LocalPosition = new_pos;
        }

        if (rjoy_down > controller_sensibility)
        {
            float move_percentage = (float)(rjoy_down - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_pos = trans.LocalPosition;
            new_pos -= vertical_thrust * move_percentage * TheTime.DeltaTime * trans.UpDirection;
            trans.LocalPosition = new_pos;
        }

        if (rjoy_right > controller_sensibility)
        {
            float move_percentage = (float)(rjoy_right - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.y -= yaw_rotate_speed * move_percentage * TheTime.DeltaTime;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.y < max_camera_rot * move_percentage && cam_rot.y >= 0.0f)
            {
                cam_rot.y += camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.y > max_camera_rot * move_percentage)
                    cam_rot.y = max_camera_rot * move_percentage;
            }
        }
        else
        {
            if (cam_rot.y > 0.0f)
            {
                cam_rot.y -= camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.y < 0.0f)
                    cam_rot.y = 0.0f;
            }
        }

        if (rjoy_left > controller_sensibility)
        {
            float move_percentage = (float)(rjoy_left - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.y += yaw_rotate_speed * move_percentage * TheTime.DeltaTime;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.y > -max_camera_rot * move_percentage && cam_rot.y <= 0.0f)
            {
                cam_rot.y -= camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.y < -max_camera_rot * move_percentage)
                    cam_rot.y = -max_camera_rot * move_percentage;
            }
        }
        else
        {
            if (cam_rot.y < 0.0f)
            {
                cam_rot.y += camera_rot_step * TheTime.DeltaTime;
                if (cam_rot.y > 0.0f)
                    cam_rot.y = 0.0f;
            }
        }

        if (right_trigger > controller_sensibility)
        {
            float move_percentage = (float)(right_trigger - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            if (curr_vel < max_vel)
                curr_vel += move_percentage * throttle_increment;
        }

        if (left_trigger > controller_sensibility)
        {
            float move_percentage = (float)(left_trigger - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            if (curr_vel > -max_vel)
                curr_vel -= move_percentage * throttle_increment;
        }

        TheVector3 new_vel_pos = trans.LocalPosition;
        new_vel_pos += trans.ForwardDirection * curr_vel * TheTime.DeltaTime;
        trans.LocalPosition = new_vel_pos;
		
		if(camera_go != null)
		{
        	camera_go.GetComponent<TheTransform>().LocalPosition = original_cam_pos + cam_pos;
        	camera_go.GetComponent<TheTransform>().LocalRotation = original_cam_rot + cam_rot;
		}
    }

    public void DamageSlaveOne(float dmg)
    {
		if(audio_source != null)
			audio_source.Play("Play_Player_hit");

        switch(TheRandom.RandomInt() % ship_parts)
        {
            case 0:
                DamageBody(dmg);
                break;
            case 1:
                DamageEngine(dmg);
                break;
            case 2:
                DamageWings(dmg);
                break;
        }
    }

    void DamageWings(float damage)
    {
        if (curr_shield_hp > 0)
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
        if (curr_shield_hp > 0)
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
        if (curr_shield_hp > 0)
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
        if(shield_regen_timer <= 0.0f && curr_shield_hp < shield_hp)
        {
            curr_shield_hp += shield_regen_energy * TheTime.DeltaTime;

            if (curr_shield_hp > shield_hp)
                curr_shield_hp = shield_hp;
        }

        shield_regen_timer -= TheTime.DeltaTime;
    }

    void RepairPuzzle()
    {
        if(repair_mode)
        {
            if (TheInput.IsKeyDown("W") && repair_ring < num_rings - 1)
            {
                repair_ring++;
            }
            if (TheInput.IsKeyDown("S") && repair_ring > 0)
            {
                repair_ring--;
            }
			
            if (TheInput.IsKeyDown("SPACE"))
            {
                switch(repair_ring)
                {
                    case 0:
                        ring_exterior_pos++;
                        ring_exterior_pos %= num_ring_pos;
						break;
                    case 1:
                        {
                            ring_center_pos++;
                            ring_center_pos %= num_ring_pos;

                            int rand = TheRandom.RandomInt() % 100;
                            if(rand < rand_rotate_pos)
                            {
                                ring_exterior_pos++;
                                ring_exterior_pos %= num_ring_pos;
                            }

                            break;
                        }
                    case 2:
                        {
                            ring_interior_pos++;
                            ring_interior_pos %= num_ring_pos;

                            int rand = TheRandom.RandomInt() % 100;
                            if (rand < rand_rotate_pos)
                            {
                                ring_exterior_pos++;
                                ring_exterior_pos %= num_ring_pos;
                            }

                            rand = TheRandom.RandomInt() % 100;
                            if (rand < rand_rotate_pos)
                            {
                                ring_center_pos++;
                                ring_center_pos %= num_ring_pos;
                            }
                            break;
                        }
                }
				
				if(inner_ring_trans != null)
					inner_ring_trans.Rotation = new TheVector3(0, 180, ring_interior_pos * -90);

				if(center_ring_trans != null)
					center_ring_trans.Rotation = new TheVector3(0, 180, ring_center_pos * -90);

				if(exterior_ring_trans != null)
					exterior_ring_trans.Rotation = new TheVector3(0, 180, ring_exterior_pos * -90);

				if(selected_inner_ring_trans != null)
					selected_inner_ring_trans.Rotation = new TheVector3(0,180,ring_interior_pos * -90);

				if(selected_center_ring_trans != null)
					selected_center_ring_trans.Rotation = new TheVector3(0,180,ring_center_pos * -90);

				if(selected_exterior_ring_trans != null)
					selected_exterior_ring_trans.Rotation = new TheVector3(0,180,ring_exterior_pos * -90);
            }
			
			switch(repair_ring)
             {
                 case 0:
					if(selected_inner_ring_trans != null)
                    	selected_inner_ring.SetActive(false);	

					if(selected_center_ring_trans != null)
						selected_center_ring.SetActive(false);

					if(selected_exterior_ring_trans != null)
						selected_exterior_ring.SetActive(true);

					break;

                 case 1:
					if(selected_inner_ring_trans != null)
                    	selected_inner_ring.SetActive(false);

					if(selected_center_ring_trans != null)
						selected_center_ring.SetActive(true);

					if(selected_exterior_ring_trans != null)
						selected_exterior_ring.SetActive(false);
                    break;    

                 case 2:
					if(selected_inner_ring_trans != null)
						selected_inner_ring.SetActive(true);
	
					if(selected_center_ring_trans != null)
						selected_center_ring.SetActive(false);

					if(selected_exterior_ring_trans != null)
						selected_exterior_ring.SetActive(false);
                    break;
             }

            if(ring_exterior_pos == 0 && ring_center_pos == 0 && ring_interior_pos == 0)
            {
                switch(repair_part)
                {
                    case 0:
                        wings_hp += repair_hp * TheTime.DeltaTime;
                        if(wings_hp >= total_hp/3.0f)
                        {
                            wings_hp = total_hp / 3.0f;
                            repair_mode = false;

							if(inner_ring != null)
                            	inner_ring.SetActive(false);

							if(center_ring != null)
                            	center_ring.SetActive(false);

							if(exterior_ring != null)
                            	exterior_ring.SetActive(false);

							if(selected_inner_ring != null)
								selected_inner_ring.SetActive(false);

							if(selected_center_ring != null)
								selected_center_ring.SetActive(false);
							
							if(selected_exterior_ring != null)
								selected_exterior_ring.SetActive(false);
                        }
                        break;
                    case 1:
                        body_hp += repair_hp * TheTime.DeltaTime;
                        if (body_hp >= total_hp / 3.0f)
                        {
                            body_hp = total_hp / 3.0f;
                            repair_mode = false;

							if(inner_ring != null)
                            	inner_ring.SetActive(false);
							
							if(center_ring != null)
                           		center_ring.SetActive(false);

							if(exterior_ring != null)
                           		exterior_ring.SetActive(false);

							if(selected_inner_ring != null)
								selected_inner_ring.SetActive(false);

							if(selected_center_ring != null)
								selected_center_ring.SetActive(false);

							if(selected_exterior_ring != null)
								selected_exterior_ring.SetActive(false);
                        }
                        break;
                    case 2:
                        engine_hp += repair_hp * TheTime.DeltaTime;
                        if (engine_hp >= total_hp / 3.0f)
                        {
                            engine_hp = total_hp / 3.0f;
                            repair_mode = false;

							if(inner_ring != null)
                            	inner_ring.SetActive(false);
				
							if(center_ring != null)
                            	center_ring.SetActive(false);

							if(exterior_ring != null)
                           		exterior_ring.SetActive(false);
								
							if(selected_inner_ring != null)
								selected_inner_ring.SetActive(false);

							if(selected_center_ring != null)
								selected_center_ring.SetActive(false);

							if(selected_exterior_ring != null)
								selected_exterior_ring.SetActive(false);
                        }
                        break;
                }
            }
        }
        else
        {
            if(TheInput.IsKeyDown("W"))
            {
                repair_part++;
				repair_part %= ship_parts;
                
            }
            if (TheInput.IsKeyDown("S"))
            {
                repair_part--;
                if (repair_part < 0)
                    repair_part = ship_parts - 1;
            }

            if(TheInput.IsKeyDown("SPACE"))
            {
                repair_mode = true;

				if(wings_part != null)
                	wings_part.SetActive(false);

				if(body_part != null)
                	body_part.SetActive(false);

				if(engine_part != null)
                	engine_part.SetActive(false);
				
				if(inner_ring != null)
					inner_ring.SetActive(true);

				if(center_ring != null)
            		center_ring.SetActive(true);
				
				if(exterior_ring != null)
            		exterior_ring.SetActive(true);

                ring_interior_pos = TheRandom.RandomInt() % 4;
                ring_center_pos = TheRandom.RandomInt() % 4;
                ring_exterior_pos = TheRandom.RandomInt() % 4;

                if (inner_ring_trans != null)
                    inner_ring_trans.Rotation = new TheVector3(0, 180, ring_interior_pos * -90);

                if (center_ring_trans != null)
                    center_ring_trans.Rotation = new TheVector3(0, 180, ring_center_pos * -90);

                if (exterior_ring_trans != null)
                    exterior_ring_trans.Rotation = new TheVector3(0, 180, ring_exterior_pos * -90);

                if (selected_inner_ring_trans != null)
                    selected_inner_ring_trans.Rotation = new TheVector3(0, 180, ring_interior_pos * -90);

                if (selected_center_ring_trans != null)
                    selected_center_ring_trans.Rotation = new TheVector3(0, 180, ring_center_pos * -90);

                if (selected_exterior_ring_trans != null)
                    selected_exterior_ring_trans.Rotation = new TheVector3(0, 180, ring_exterior_pos * -90);
            }

            switch(repair_part)
            {
                case 0:
					if(wings_part != null)
                    	wings_part.SetActive(true);
			
					if(body_part != null)
                   		body_part.SetActive(false);

					if(engine_part != null)
                    	engine_part.SetActive(false);
                    break;
                case 1:
					if(wings_part != null)
                   		wings_part.SetActive(false);
		
					if(body_part != null)
                   		body_part.SetActive(true);

					if(engine_part != null)
                    	engine_part.SetActive(false);
                    break;
                case 2:
					if(wings_part != null)
                    	wings_part.SetActive(false);

					if(body_part != null)
                    	body_part.SetActive(false);

					if(engine_part != null)
                    	engine_part.SetActive(true);
                    break;
            }
        }
    }

}