using TheEngine;
using TheEngine.TheConsole;

public class PlayerMovement {

	// Self transform
	private TheTransform trans;
	
	// Controller Settings
	public int controller_sensibility = 2500;
	public int triggger_sensibility = 0;
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
	///boost properties
	public float boost_extra_vel = 25.0f;
	public float boost_accel_multiplier = 1.5f;
	public float boost_time = 1.5f;
	public float boost_cd_time = 5.0f;
	///slow (still not decided)
	public float slow_time = 1.0f;
	public float slow_regen = 0.1f;
	
	// Ship Private Information
	///ship speeds
	private float curr_vel = 0.0f;
	private float vel_percent = 0.02f; //from 1.0f to 0.02f;
	private float curr_max_vel = 10.0f;
	private float curr_accel;
	///slow
	private bool slowing = false;
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
    public TheGameObject shield_hp_go;
    public TheGameObject hp;
	///Element Component
    private TheProgressBar weapons_bar = null;
	private TheProgressBar shields_bar = null;
	private TheProgressBar energy_bar = null;
	private TheProgressBar speed_bar = null;
    private TheProgressBar shield_hp_bar = null;
    private TheProgressBar hp_bar = null;
	
	//Repair Puzzle
	private int num_ring_pos = 4;
    private int repair_ring = 0;
    private int num_rings = 3;
    private int repair_part = 0;
    private int ship_parts = 3;
    private bool repair_mode = false;
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

	void Start () 
	{
		//Get the transform
		trans = TheGameObject.Self.GetComponent<TheTransform>();
		
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
				audio_source.SetMyRTPCvalue("Speed",vel_percent);
			}
		}
		
		//Particle Emitter
		if(particle_emitter_go != null)
		{
			particle_emitter = particle_emitter_go.GetComponent<TheParticleEmmiter>();
		}
	}
	
	void Update () 
	{
		delta_time = TheTime.DeltaTime;
		
	}
	
	void Movement()
	{
		
	}
	
	void SetValuesFromEnergy()
	{
		// Calculate current max velocity form energy_bar
		curr_max_vel = 0.5f * max_vel + ((12.5f*(float)engine_energy)/100) * max_vel;
		// Calculate current shield regen from energy_bar
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
	
	
}