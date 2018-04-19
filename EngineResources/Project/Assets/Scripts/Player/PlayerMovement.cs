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
	TheAudioSource audio_source;
	

	void Start () {
		
	}
	
	void Update () {
		
	}
}