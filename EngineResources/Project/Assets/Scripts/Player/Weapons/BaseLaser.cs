using TheEngine;
using TheEngine.TheMath;
using TheEngine.TheConsole;

public class BaseLaser 
{
	// other scripts and go information
	private TheGameObject game_manager = null;
	private TheGameObject slave_go = null;
	private TheTransform slave_transform = null;	
	private TheScript weapon_manager = null;
	
	//spawns
	public TheGameObject laser_spawn_1;
	private TheTransform laser_spawn_1_trans;
	public TheGameObject laser_spawn_2;
	private TheTransform laser_spawn_2_trans;
	private bool spawn1_used = false;
	
	//weapon info
	public int damage = 10;
	public float speed = 1000;
	private float overheat = 0.0f;
	private float shoot_timer = 0.0f;
	public float light_duration = 0.5f;
	
	//factory
	TheFactory laser_factory;
	public float laser_dir_distance = 10000.0f;
	
	
	void Init()
	{
		game_manager = TheGameObject.Find("GameManager");

		weapon_manager = TheGameObject.Self.GetScript("PlayerWeaponManager");		
	}
	
	void Start () 
	{
		if(game_manager != null)
		{
			TheScript game_manager_script = game_manager.GetScript("GameManager");

			if(game_manager_script != null)
			{
				slave_go = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");

				if(slave_go != null)
				{
					slave_transform = slave_go.GetComponent<TheTransform>();
				}
			}
		}
		
		if(laser_spawn_1 != null)
			laser_spawn_1_trans = laser_spawn_1.GetComponent<TheTransform>();
		
		if(laser_spawn_2 != null)
			laser_spawn_2_trans = laser_spawn_2.GetComponent<TheTransform>();
		
		laser_factory = (TheFactory)weapon_manager.CallFunctionArgs("GetFactory");
	}
	
	void Update () 
	{
		
	}
	
	float ShootPress(float weapon_energy)
	{
		if(shoot_timer <= 0.0f)
		{
			//shoot
			TheVector3 shoot_pos;
			
			if(spawn1_used)
			{
				shoot_pos = laser_spawn_2_trans.GlobalPosition;
				spawn1_used = false;
			}
			else
			{
				shoot_pos = laser_spawn_1_trans.GlobalPosition;
				spawn1_used = true;
			}
			
			if(laser_factory != null)
			{
				TheGameObject go = laser_factory.Spawn();
				
				if(go != null)
				{
					TheScript laser_script = go.GetScript("Laser");
					TheTransform laser_trans = go.GetComponent<TheTransform>();
					if(laser_script != null && slave_transform != null && slave_go != null && laser_trans != null)
					{
						//calculate the target point
						TheVector3 target = slave_transform.GlobalPosition + slave_transform.ForwardDirection * laser_dir_distance;
					
						TheVector3 laser_dir = target - shoot_pos;
						laser_dir = TheVector3.Normalize(laser_dir);
						
						laser_trans.GlobalPosition = shoot_pos;
						
						object[] args = {slave_go, speed, damage, laser_dir, slave_transform.QuatRotation};
						laser_script.CallFunctionArgs("SetInfo", args);
						
						string audio = "Play_shot";
						object[] audio_args = {audio};
						
						weapon_manager.CallFunctionArgs("PlayAudio", audio_args);
						
						TheConsole.Log("FIRE!");
					}
				}
			}
			
			shoot_timer = -75.16412f + (0.2187436f + 75.16412f)/(1 + TheMath.Pow(weapon_energy/381393900, 0.4224618f));
		}
		
		shoot_timer -= TheTime.DeltaTime;
		
		return 0.0f; //this weapon doesn't overheat the weapon
	}
	
	void ShootRelease()
	{
		
	}
	
}