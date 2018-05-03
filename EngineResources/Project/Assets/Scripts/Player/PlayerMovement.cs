using TheEngine;
using TheEngine.TheConsole; 

public class PlayerMovement 
{
	// Self transform
	private TheTransform trans;
	
	private bool can_move = true;

	// Controller Settings
	public int controller_sensibility = 2500;
	public int trigger_sensibility = 0;
	public bool invert_axis = false;
	public float rotate_rumble_strength = 0.05f;
	public float accel_max_rumble_strength = 0.3f;
	public float boost_rumble_strength = 0.3f;
	public int rotate_rumble_ms = 5;
	public int accel_rumble_ms = 3;
	public int boost_rumble_ms = 3;
	
	// Ship Public Information
	///ship rotation speeds
	public float roll_rotate_speed = 45.0f;
	public float pitch_rotate_speed = 40.0f;
	public float yaw_rotate_speed = 25.0f;
	///ship speed modifiers
	public float max_vel = 50.0f;
	public float min_vel = -5.0f;
	public float acceleration = 10.0f;
	public float slow_acceleration = 50.0f;
	public float vertical_thrust = 20.0f;
	public float default_speed_percent = 0.1f;
	public float default_break_percent = 0.1f;
	///boost properties
	private bool boosting = false;
	public float boost_extra_vel = 25.0f;
	public float boost_accel_multiplier = 1.5f;
	public float boost_time = 1.5f;
	public float boost_cd_time = 5.0f;
	//controls
	public string roll_pos_joystic = "RIGHTJOY_RIGHT";
	public string roll_neg_joystic = "RIGHTJOY_LEFT";
	public string pitch_pos_joystic = "LEFTJOY_UP";
	public string pitch_neg_joystic = "LEFTJOY_DOWN";
	public string yaw_pos_joystic = "LEFTJOY_RIGHT";
	public string yaw_neg_joystic = "LEFTJOY_LEFT";
	public string accel_joystic = "RIGHT_TRIGGER";
	public string break_joystick = "LEFT_TRIGGER";
	public string vertical_movement_up_joystic = "RIGHTJOY_UP";
	public string vertical_movement_down_joystic = "RIGHTJOY_DOWN";
	public string boost_button = "CONTROLLER_L3";
	public string front_shield_key = "E";
	public string back_shield_key = "D";
	
	
	//Energy Controls
	public string energy_balance_keyboard = "DOWN_ARROW";
	public string energy_attack_keyboard = "LEFT_ARROW";
	public string energy_shields_keyboard = "UP_ARROW";
	public string energy_engine_keyboard = "RIGHT_ARROW";
	
	public string energy_balance_controller = "CONTROLLER_DOWN_ARROW";
	public string energy_attack_controller = "CONTROLLER_LEFT_ARROW";
	public string energy_shields_controller = "CONTROLLER_UP_ARROW";
	public string energy_engine_controller = "CONTROLLER_RIGHT_ARROW";
	
	// Ship Private Information
	///ship speeds
	private float curr_vel = 0.0f;
	private float vel_percent = 0.02f; //from 1.0f to 0.02f;
	private float curr_max_vel = 10.0f;
	private float curr_accel;
	private float curr_decel;
	private float decel_percent = 0.1f;
	///slow
	private bool breaking = false;
	/// camera movement
	public float roll_camera_rot = 10.0f;
	public float pitch_camera_rot = 10.0f;
	public float yaw_camera_rot = 10.0f;
	public float max_camera_displacement = 0.1f;
    public float camera_rot_step = 1.0f;
	public float camera_return_step = 2.0f;
    public TheGameObject camera_go;
    private TheVector3 original_cam_rot;
    private TheVector3 original_cam_pos;
    private TheVector3 cam_rot;
    private TheVector3 cam_pos;
	//hp and shield
	private float total_hp = 100.0f;
	private float curr_total_hp;
	private float body_hp;
	private float wings_hp;
	private float engine_hp;
	public float shield_hp = 25.0f;
	private float curr_shield_hp;
	public float shield_regen = 10.0f;
    public float shield_regen_time = 10.0f;
    private float shield_regen_energy;
    private float shield_regen_timer;
	private int front_shield = 2;
	private int back_shield = 2;
	private float shield_step = 0.25f;
	
	
	// Ship Information Timers
	///boost
	private float boost_timer;
	private float boost_cd_timer;
	///slow
	private float slow_timer;
	
	//Energy Management
	public int max_energy = 6;
	public int shield_energy = 2;
	public int weapon_energy = 2;
	public int engine_energy = 2;
	public int max_energy_on_system = 4;
	
	//UI elements
	/// GameObjects
	public TheGameObject weapons;
	public TheGameObject shields;
	public TheGameObject energy;
	public TheGameObject speed;
    //public TheGameObject shield_hp_go;
    public TheGameObject hp;
	///Shields
	public TheGameObject front_shield_25;
	public TheGameObject front_shield_50;
	public TheGameObject front_shield_75;
	public TheGameObject front_shield_100;
	public TheGameObject back_shield_25;
	public TheGameObject back_shield_50;
	public TheGameObject back_shield_75;
	public TheGameObject back_shield_100;
	public TheGameObject shield_green;
	public TheGameObject shield_yellow;
	public TheGameObject shield_red;
	public float shield_yellow_value = 15.0f;
	public float shield_red_value = 7.0f;
	
	///Element Component
    private TheProgressBar weapons_bar = null;
	private TheProgressBar shields_bar = null;
	private TheProgressBar energy_bar = null;
	private TheProgressBar speed_bar = null;
    //private TheProgressBar shield_hp_bar = null;
    private TheProgressBar hp_bar = null;
	
	//Repair Puzzle
	private int num_ring_pos = 4;
    private int repair_ring = 0;
    private int num_rings = 3;
    private int repair_part = 0;
    private int ship_parts = 3;
    private bool repair_mode = false;
	public int rand_rotate_pos = 50;
    public float repair_hp = 5.0f;
	/// rings positions
	private int ring_exterior_pos = 0;
    private int ring_center_pos = 0;
    private int ring_interior_pos = 0;
	/// UI Elements
	/// GameObjects
	public TheGameObject inner_ring;
    public TheGameObject center_ring;
    public TheGameObject exterior_ring;
	public TheGameObject selected_inner_ring;
	public TheGameObject selected_center_ring;
	public TheGameObject selected_exterior_ring;
	public TheGameObject body_part;
    public TheGameObject wings_part;
    public TheGameObject engine_part;
	/// Element Component
	private TheRectTransform inner_ring_trans;
    private TheRectTransform center_ring_trans;
    private TheRectTransform exterior_ring_trans;
	private TheRectTransform selected_inner_ring_trans;
    private TheRectTransform selected_center_ring_trans;
    private TheRectTransform selected_exterior_ring_trans;
	
	//Audio
	public TheGameObject audio_emiter;
	private TheAudioSource audio_source;
	
	//Particle Emitter for speed fx
	public TheGameObject particle_emitter_go;
	private TheParticleEmmiter particle_emitter = null;
	public float particle_base_speed = 1.0f;
	
	//Current frame delta time
	private float delta_time = 0.0f;
	
	//ShipProperties Script
	private TheScript ship_properties = null;
	
	//Dead check
	private bool is_dead = false;
	
	TheGameObject self = null;

	void Start () 
	{
		self = TheGameObject.Self;
		
		//Get the transform
		trans = self.GetComponent<TheTransform>();
		
		//Get all the Components needed for the UI elements
		/// Bars
		if(weapons != null)
			weapons_bar = weapons.GetComponent<TheProgressBar>();
	
		if(shields != null)
			shields_bar = shields.GetComponent<TheProgressBar>();

		if(energy != null)
			energy_bar = energy.GetComponent<TheProgressBar>();

		if(speed != null)
			speed_bar = speed.GetComponent<TheProgressBar>();

		//if(shield_hp_go != null)
        	//shield_hp_bar = shield_hp_go.GetComponent<TheProgressBar>();

		if(hp != null)
       		hp_bar = hp.GetComponent<TheProgressBar>();

		if(weapons_bar != null)
       		weapons_bar.PercentageProgress = 100;

		if(shields_bar != null)
			shields_bar.PercentageProgress = 100;

		if(energy_bar != null)
			energy_bar.PercentageProgress = 100;
		/// Images
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
		
		//Energy System Start
		max_energy *= 2;
		shield_energy *= 2;
		weapon_energy *= 2;
		engine_energy *= 2;
		max_energy_on_system *= 2;
		
		//Set initial values for velocity and acceleration
		curr_accel = acceleration;
		curr_max_vel = max_vel;
		
		//HP and shield setup
		body_hp = wings_hp = engine_hp = total_hp/3.0f;
		curr_total_hp = total_hp;
		curr_shield_hp = shield_hp;
        shield_regen_energy = shield_regen;
		
		//Camera information
		if(camera_go != null)
		{
        	original_cam_pos = camera_go.GetComponent<TheTransform>().LocalPosition;
        	original_cam_rot = camera_go.GetComponent<TheTransform>().LocalRotation;
		}
        cam_pos = new TheVector3(0, 0, 0);
        cam_rot = new TheVector3(0, 0, 0);
		
		//Audio 
		if(audio_emiter != null)
		{
       		audio_source = audio_emiter.GetComponent<TheAudioSource>();

			if(audio_source != null)
			{
				audio_source.Play("Play_Engine");
				audio_source.SetMyRTPCvalue("Velocity",vel_percent);
			}
		}
		
		//Particle Emitter
		if(particle_emitter_go != null)
		{
			particle_emitter = particle_emitter_go.GetComponent<TheParticleEmmiter>();
		}
		
		//Get ShipProperties Script
		ship_properties = self.GetScript("EntityProperties");
	}
	
	void Update () 
	{
		//Get delta_time for this Update from engine
		delta_time = TheTime.DeltaTime;
		
		//Execute the logic for the player
		if(can_move)
		{
			Movement();
			EnergyManagement();
			RepairPuzzle();
			RegenShield();
			ManageShields();
		}
		
		
		//Apply changes to values
		SetValuesFromEnergy();
		SetUIElements();
		SetParticlesValues();
		
		//Calculate current hp and send to ship properties script
		curr_total_hp = wings_hp + body_hp + engine_hp;
		if(ship_properties != null)
		{
			object[] args = {(int)curr_total_hp};
			if(is_dead) args[0] = 0;
			ship_properties.CallFunctionArgs("SetLife", args);
		}
		
	}
	
	void SetCanMove(bool set)
	{
		can_move = set;
	}

	void Movement()
	{
		int rjoy_up = TheInput.GetControllerJoystickMove(0, vertical_movement_up_joystic);
        int rjoy_down = TheInput.GetControllerJoystickMove(0, vertical_movement_down_joystic);
        int rjoy_right = TheInput.GetControllerJoystickMove(0, yaw_pos_joystic);
        int rjoy_left = TheInput.GetControllerJoystickMove(0, yaw_neg_joystic);

        int ljoy_up = TheInput.GetControllerJoystickMove(0, pitch_pos_joystic);
        int ljoy_down = TheInput.GetControllerJoystickMove(0, pitch_neg_joystic);
        int ljoy_right = TheInput.GetControllerJoystickMove(0, roll_pos_joystic);
        int ljoy_left = TheInput.GetControllerJoystickMove(0, roll_neg_joystic);

        int right_trigger = TheInput.GetControllerJoystickMove(0, accel_joystic);
        int left_trigger = TheInput.GetControllerJoystickMove(0, break_joystick);
		
		if (ljoy_up > controller_sensibility)
        {
            float move_percentage = (float)(ljoy_up - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
			if(invert_axis)
			{
				new_rot.x -= pitch_rotate_speed * move_percentage * delta_time;
				trans.SetIncrementalRotation(new_rot);
				if (cam_rot.x > -pitch_camera_rot * move_percentage && cam_rot.x <= 0.0f)
                {
                    cam_rot.x -= camera_rot_step * delta_time;
                    if (cam_rot.x < -pitch_camera_rot * move_percentage)
                    {
                        cam_rot.x = -pitch_camera_rot * move_percentage;
                    }
                }
			}
			else
			{
				new_rot.x += pitch_rotate_speed * move_percentage * delta_time;
				trans.SetIncrementalRotation(new_rot);
				if (cam_rot.x < pitch_camera_rot * move_percentage && cam_rot.x >= 0.0f)
				{
						cam_rot.x += camera_rot_step * delta_time;
						if (cam_rot.x > pitch_camera_rot * move_percentage)
							cam_rot.x = pitch_camera_rot * move_percentage;
				}
			}
		}
		else
        {
			if(invert_axis)
			{
				if (cam_rot.x < 0.0f)
				{
					cam_rot.x += camera_return_step * delta_time;
					if (cam_rot.x > 0.0f)
						cam_rot.x = 0.0f;
				}
			}
			else
			{
				if (cam_rot.x > 0.0f)
                {
                    cam_rot.x -= camera_return_step * delta_time;
                    if (cam_rot.x < 0.0f)
                        cam_rot.x = 0.0f;
                }
			}
		}
		
		if (ljoy_down > controller_sensibility)
        {
            float move_percentage = (float)(ljoy_down - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.x -= pitch_rotate_speed * move_percentage * delta_time;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.x > -pitch_camera_rot * move_percentage && cam_rot.x <= 0.0f)
                {
                    cam_rot.x -= camera_rot_step * delta_time;
                    if (cam_rot.x < -pitch_camera_rot * move_percentage)
                    {
                        cam_rot.x = -pitch_camera_rot * move_percentage;
                    }
                }
        }
		else
        {
            
            if (cam_rot.x < 0.0f)
            {
                cam_rot.x += camera_return_step * delta_time;
                if (cam_rot.x > 0.0f)
                    cam_rot.x = 0.0f;
            }
            
        }
		
		if (ljoy_right > controller_sensibility)
        {
            float move_percentage = (float)(ljoy_right - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.z += roll_rotate_speed * move_percentage * delta_time;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.z > -roll_camera_rot && cam_rot.z <= 0.0f)
            {
                cam_rot.z -= camera_rot_step * delta_time;
                if (cam_rot.z < -roll_camera_rot)
                    cam_rot.z = -roll_camera_rot;
            }
		}
        else
        {
            if(cam_rot.z<0.0f)
            {
                cam_rot.z += camera_return_step * delta_time;
                if (cam_rot.z > 0.0f)
                    cam_rot.z = 0.0f;
            }
        }

        if (ljoy_left > controller_sensibility)
        {
            float move_percentage = (float)(ljoy_left - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.z -= roll_rotate_speed * move_percentage * delta_time;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.z < roll_camera_rot && cam_rot.z >= 0.0f)
            {
                cam_rot.z += camera_rot_step * delta_time;
                if (cam_rot.z > roll_camera_rot)
                    cam_rot.z = roll_camera_rot;
            }
        }
        else
        {
            if (cam_rot.z > 0.0f)
            {
                cam_rot.z -= camera_return_step * delta_time;
                if (cam_rot.z < 0.0f)
                    cam_rot.z = 0.0f;
            }
        }

        if (rjoy_up > controller_sensibility)
        {
            float move_percentage = (float)(rjoy_up - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_pos = trans.LocalPosition;
            new_pos += vertical_thrust * move_percentage * delta_time * trans.UpDirection;
            trans.LocalPosition = new_pos;
        }

        if (rjoy_down > controller_sensibility)
        {
            float move_percentage = (float)(rjoy_down - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_pos = trans.LocalPosition;
            new_pos -= vertical_thrust * move_percentage * delta_time * trans.UpDirection;
            trans.LocalPosition = new_pos;
        }

        if (rjoy_right > controller_sensibility)
        {
            float move_percentage = (float)(rjoy_right - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.y -= yaw_rotate_speed * move_percentage * delta_time;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.y < yaw_camera_rot * move_percentage && cam_rot.y >= 0.0f)
            {
                cam_rot.y += camera_rot_step * delta_time;
                if (cam_rot.y > yaw_camera_rot * move_percentage)
                    cam_rot.y = yaw_camera_rot * move_percentage;
            }
        }
        else
        {
            if (cam_rot.y > 0.0f)
            {
                cam_rot.y -= camera_return_step * delta_time;
                if (cam_rot.y < 0.0f)
                    cam_rot.y = 0.0f;
            }
        }

        if (rjoy_left > controller_sensibility)
        {
            float move_percentage = (float)(rjoy_left - controller_sensibility) / (float)(TheInput.MaxJoystickMove - controller_sensibility);
            TheVector3 new_rot = trans.LocalRotation;
            new_rot.y += yaw_rotate_speed * move_percentage * delta_time;
            trans.SetIncrementalRotation(new_rot);
			if (cam_rot.y > -yaw_camera_rot * move_percentage && cam_rot.y <= 0.0f)
            {
                cam_rot.y -= camera_rot_step * delta_time;
                if (cam_rot.y < -yaw_camera_rot * move_percentage)
                    cam_rot.y = -yaw_camera_rot * move_percentage;
            }
        }
        else
        {
            if (cam_rot.y < 0.0f)
            {
                cam_rot.y += camera_return_step * delta_time;
                if (cam_rot.y > 0.0f)
                    cam_rot.y = 0.0f;
            }
        }
		
		vel_percent = default_speed_percent; //reset to min vel
		if(right_trigger > trigger_sensibility)
		{
			vel_percent = (float)(right_trigger - trigger_sensibility)/(float)(TheInput.MaxJoystickMove - trigger_sensibility);
			if(vel_percent<default_speed_percent) vel_percent = default_speed_percent;
		}
		
		decel_percent = default_break_percent;
		breaking = false;
		if (left_trigger > controller_sensibility)
        {
            decel_percent = (float)(left_trigger - trigger_sensibility)/(float)(TheInput.MaxJoystickMove - trigger_sensibility);
			if(decel_percent<default_break_percent)
			{	
				decel_percent = default_break_percent;
				
			}
			else
				breaking = true;
			
        }
		
		if(TheInput.GetControllerButton(0,boost_button) == 2 && boost_cd_timer <= 0.0f)
		{
			boosting = true;
			boost_timer = boost_time;
			boost_cd_timer = 0.1f; //dont allow to boost continously
		}
		
		float target_vel = vel_percent * curr_max_vel;
		
		if(audio_source != null)
			audio_source.SetMyRTPCvalue("Velocity",(curr_vel/((1.5f * max_vel) + boost_extra_vel))*100);
		
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
				boost_timer -= delta_time;
		}
		else
		{
			if(engine_energy > 4)
			{
				boost_cd_timer -= delta_time*(1+(0.5f/4) * (engine_energy-4));
			}
			else if(engine_energy < 4)
			{
				boost_cd_timer -= delta_time*(1-(0.75f/4) * (engine_energy));
			}
			else
				boost_cd_timer -= delta_time;
		
		}
		
		if(curr_vel < target_vel) 
		{
			curr_vel += curr_accel*delta_time;

			float rumble = accel_max_rumble_strength - (curr_vel/target_vel)*accel_max_rumble_strength;

			TheInput.RumbleController(0,rumble,accel_rumble_ms);
		}
		else if(curr_vel > target_vel)
		{
			if(breaking)
			{
				curr_vel -= decel_percent*slow_acceleration*delta_time;
				float rumble = accel_max_rumble_strength - (target_vel/curr_vel)*accel_max_rumble_strength;
				TheInput.RumbleController(0,rumble,accel_rumble_ms);
			}
			else
			{
				curr_vel -= acceleration*delta_time;
			}		
		}
		
		TheVector3 new_vel_pos = trans.LocalPosition;
		new_vel_pos += trans.ForwardDirection*curr_vel*delta_time;
		trans.LocalPosition = new_vel_pos;
		
		if(camera_go != null)
		{
        	camera_go.GetComponent<TheTransform>().LocalPosition = original_cam_pos + cam_pos;
        	camera_go.GetComponent<TheTransform>().LocalRotation = original_cam_rot + cam_rot;
		}
	}
	
	void SetValuesFromEnergy()
	{
		// Calculate current max velocity form energy_bar
		curr_max_vel = 0.5f * max_vel + ((12.5f*(float)engine_energy)/100) * max_vel;
		// Calculate current shield regen from energy_bar
        shield_regen_energy = 0.5f * shield_regen + ((12.5f * (float)shield_energy) / 100) * shield_regen;
    }
	
	void SetUIElements()
	{
		if(weapons_bar != null)
			weapons_bar.PercentageProgress = (100.0f / 8.0f) * weapon_energy;
		
		if(shields_bar != null)
			shields_bar.PercentageProgress = (100.0f / 8.0f) * shield_energy;

		if(energy_bar != null)
			energy_bar.PercentageProgress = (100.0f / 8.0f) * engine_energy;
		
		//if(shield_hp_bar != null)
        	//shield_hp_bar.PercentageProgress = (curr_shield_hp / shield_hp) * 100.0f;

		if(hp_bar != null)
        	hp_bar.PercentageProgress = (curr_total_hp / total_hp) * 100.0f;

		if(speed_bar != null)
			speed_bar.PercentageProgress = (curr_vel/((1.5f * max_vel) + boost_extra_vel))  * 100;
		
		//Shields
		if(front_shield_25 != null && front_shield_50 != null && front_shield_75 != null && front_shield_100 != null)
		{
			//Set ui elements acording to values
			front_shield_25.SetActive(false);
			front_shield_50.SetActive(false);
			front_shield_75.SetActive(false);
			front_shield_100.SetActive(false);
		
			switch(front_shield)
			{
				case 4:
					front_shield_100.SetActive(true);
					front_shield_75.SetActive(true);
					front_shield_50.SetActive(true);
					front_shield_25.SetActive(true);
					break;
				case 3:
					front_shield_75.SetActive(true);
					front_shield_50.SetActive(true);
					front_shield_25.SetActive(true);
					break;
				case 2:
					front_shield_50.SetActive(true);
					front_shield_25.SetActive(true);
					break;
				case 1:
					front_shield_25.SetActive(true);
					break;
				case 0:
					break;
				
			}
		}
		
		if(back_shield_25 != null && back_shield_50 != null && back_shield_75 != null && back_shield_100 != null)
		{
			back_shield_25.SetActive(false);
			back_shield_50.SetActive(false);
			back_shield_75.SetActive(false);
			back_shield_100.SetActive(false);
		
			switch(back_shield)
			{
				case 4:
					back_shield_100.SetActive(true);
					back_shield_75.SetActive(true);
					back_shield_50.SetActive(true);
					back_shield_25.SetActive(true);
					break;
				case 3:
					back_shield_75.SetActive(true);
					back_shield_50.SetActive(true);
					back_shield_25.SetActive(true);
					break;
				case 2:
					back_shield_50.SetActive(true);
					back_shield_25.SetActive(true);
					break;
				case 1:
					back_shield_25.SetActive(true);
					break;
				case 0:
					break;
				
			}
		}
		
		if(shield_green != null && shield_yellow != null && shield_red != null)
		{
			//Update Shield Generator HP image 
			shield_green.SetActive(false);
			shield_yellow.SetActive(false);
			shield_red.SetActive(false);
		
			if(shield_hp > shield_yellow_value)
			{
				shield_green.SetActive(true);
			}
			else if(shield_hp > shield_red_value)
			{
				shield_yellow.SetActive(true);
			}
			else if(shield_hp > 0.0f)
				shield_red.SetActive(false);
		}
	}
	
	void SetParticlesValues()
	{
		if(particle_emitter != null)
		{	
			particle_emitter.SetEmitterSpeed(((curr_vel/((1.5f * max_vel) + boost_extra_vel))  * 100)* particle_base_speed);
			particle_emitter.SetParticleSpeed(((curr_vel/((1.5f * max_vel) + boost_extra_vel))  * 100)* particle_base_speed);
        }
	}
	
	void EnergyManagement()
	{
		if(TheInput.IsKeyDown(energy_shields_keyboard) || TheInput.GetControllerButton(0, energy_shields_controller) == 1)
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
		
		if(TheInput.IsKeyDown(energy_attack_keyboard) || TheInput.GetControllerButton(0, energy_attack_controller) == 1)
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

		if(TheInput.IsKeyDown(energy_engine_keyboard) || TheInput.GetControllerButton(0, energy_engine_controller) == 1)
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

		if(TheInput.IsKeyDown(energy_balance_keyboard) || TheInput.GetControllerButton(0, energy_balance_controller) == 1)
		{
			audio_source.Stop("Play_droid_speed_down");
			audio_source.Play("Play_droid_speed_down");

			shield_energy = 4;
			weapon_energy = 4;
			engine_energy = 4;
		}

		if(TheInput.IsKeyDown("F1"))
		{
			DamageSlaveOne(10);
		}
	}
	
	void ManageShields()
	{
		if(TheInput.IsKeyDown(front_shield_key) && front_shield < 4)
		{
			front_shield++;
			back_shield--;
		}
		if(TheInput.IsKeyDown(back_shield_key) && back_shield < 4)
		{
			front_shield--;
			back_shield++;
		}

	}
	
	void DamageFrontShield(int dmg)
	{
		if(shield_hp > 0)
		{
			float shield_dmg = (float)dmg*(4-front_shield)*shield_step;
			
			shield_hp -= shield_dmg;
			if(shield_hp < 0)
				shield_hp = 0;
		}
		else
			DamageSlaveOne(dmg);
	}
	
	void DamageBackShield(int dmg)
	{
		if(shield_hp > 0)
		{
			float shield_dmg = (float)dmg*(4-back_shield)*shield_step;
			
			shield_hp -= shield_dmg;
			if(shield_hp < 0)
				shield_hp = 0;
		}
		else
			DamageSlaveOne(dmg);
	}
	
	void DamageSlaveOne(int dmg)
    {
		float f_dmg = dmg;
		
		if(audio_source != null)
			audio_source.Play("Play_Ship_hit");

		//TheConsole.Log("Damage Slave One: " + dmg);

        switch(TheRandom.RandomInt() % ship_parts)
        {
            case 0:
				//TheConsole.Log("Slave body damaged");
                DamageBody(f_dmg);
                break;
            case 1:
				//TheConsole.Log("Slave engine damaged");
                DamageEngine(f_dmg);
                break;
            case 2:
				//TheConsole.Log("Slave wings damaged");
                DamageWings(f_dmg);
                break;
        }
    }

    void DamageWings(float damage)
    {
        wings_hp -= damage;

        if (wings_hp <= 0.0f)
        {
			is_dead = true;
        }
        
    }

    void DamageBody(float damage)
    {
        body_hp -= damage;

        if (wings_hp <= 0.0f)
        {
            is_dead = true;
        }
        
    }

    void DamageEngine(float damage)
    {
        engine_hp -= damage;
        if (wings_hp <= 0.0f)
        {
            is_dead = true;
        }
        
    }
	
	void RegenShield()
    {
        if(shield_regen_timer <= 0.0f && curr_shield_hp < shield_hp)
        {
            curr_shield_hp += shield_regen_energy * delta_time;

            if (curr_shield_hp > shield_hp)
                curr_shield_hp = shield_hp;
        }

        shield_regen_timer -= delta_time;
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
                        wings_hp += repair_hp * delta_time;
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
                        body_hp += repair_hp * delta_time;
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
                        engine_hp += repair_hp * delta_time;
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

            if(TheInput.IsKeyDown("SPACE") && !PartFull(repair_part))
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
	
	bool PartFull(int part)
	{
		bool ret = true;
		TheConsole.Log(wings_hp + body_hp + engine_hp);
		TheConsole.Log(total_hp/3.0f);
		switch(repair_part)
		{
			case 0:
				if(wings_hp < (total_hp/3.0f)-0.1f)
					ret = false;
				break;
			case 1:
				if(body_hp < (total_hp/3.0f)-0.1f)
					ret = false;
				break;
			case 2:
				if(engine_hp < (total_hp/3.0f)-0.1f)
					ret = false;
				break;
		}
		TheConsole.Log(ret);
		return ret;		
	}
}