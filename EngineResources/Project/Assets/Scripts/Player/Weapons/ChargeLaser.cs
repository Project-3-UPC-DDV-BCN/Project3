using TheEngine;
using TheEngine.TheMath;
using TheEngine.TheConsole;

public class ChargeLaser 
{
	// other scripts and go information

	public float charge_factor = 0.5f; // used for the charging fx
	private TheGameObject game_manager = null;
	private TheGameObject slave_go = null;
	private TheTransform slave_transform = null;	
	private TheScript weapon_manager = null;
	public TheGameObject slave_emmiter;
	private TheAudioSource slave_audio = null;
	
	//spawns
	public TheGameObject laser_spawn;
	private TheTransform laser_spawn_trans;
	private bool charge_fx = false;
	private bool release_charge = false;
	float charge = 50;

	
	//weapon info
	public int total_damage = 10;
	public float speed = 1000;
	public float overheat_increment = 0.2f;
	private float curr_overheat_inc;
	private float overheat = 0.0f;
	public float light_duration = 0.5f;
	private bool overheated = false;
	private bool charging = false;
	private bool cooling = false;
	
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
		
		if(laser_spawn != null)
			laser_spawn_trans = laser_spawn.GetComponent<TheTransform>();
		
		if(slave_emmiter != null)
			slave_audio = slave_emmiter.GetComponent<TheAudioSource>();

		laser_factory = (TheFactory)weapon_manager.CallFunctionArgs("GetFactory");
		
		curr_overheat_inc = overheat_increment;
	}
	
	void Update () 
	{
		if(charge_fx && !cooling)
		{
			charge += charge_factor;
			slave_audio.SetMyRTPCvalue("Charge_Percentatge",charge);
		}
		if(release_charge)		
		{
			if(charge > 0)
			{
				charge-=2;
				slave_audio.SetMyRTPCvalue("Charge_Percentatge", charge);
			}
			else
			{
				slave_audio.Stop("Play_Charge");
				charge_fx = false;
				release_charge = false;
				charge = 50;
			}
		}

		if(!charging && overheat > 0.0f)
		{
			overheat -=curr_overheat_inc*TheTime.DeltaTime;
			
			if(overheat <=0.0f)
			{
				overheat = 0.0f;
				overheated = false;
				cooling = false;
			}
			
			object[] args =  {overheat};
			weapon_manager.CallFunctionArgs("SetOverheat", args);
		}
	}
	
	float ShootPress(float weapon_energy)
	{
		if (cooling)
			return overheat;
		
		if(!charge_fx)
		{
			slave_audio.Play("Play_Charge");
			charge_fx = true;
		}

		curr_overheat_inc = overheat_increment + overheat_increment * (weapon_energy / 100.0f);
		
		if(!overheated)
		{
			overheat += curr_overheat_inc*TheTime.DeltaTime;
			
			if(overheat > 1.0f)
				overheat = 1.0f;
			
			charging = true;
		}
		
		return overheat; 
	}
	
	void ShootRelease()
	{
		if (cooling)
			return;
		
		release_charge = true;
		charge_fx = false;
		if(!overheated)
		{
			//shoot
			TheVector3 shoot_pos;
			
			shoot_pos = laser_spawn_trans.GlobalPosition;
			
			if(laser_factory != null)
			{
				TheGameObject go = laser_factory.Spawn();
				TheConsole.Log("Shoot Release - factory");
				
				if(go != null)
				{
					TheConsole.Log("Shoot Release - go");
					TheScript laser_script = go.GetScript("Laser");
					TheTransform laser_trans = go.GetComponent<TheTransform>();
					if(laser_script != null && slave_transform != null && slave_go != null && laser_trans != null)
					{
						TheConsole.Log("Shoot Release - info");
						//calculate the target point
						TheVector3 target = slave_transform.GlobalPosition + slave_transform.ForwardDirection * laser_dir_distance;
						TheVector3 scale = new TheVector3(1.0f+overheat,1.0f+overheat,1.0f);
					
						TheVector3 laser_dir = target - shoot_pos;
						laser_dir = TheVector3.Normalize(laser_dir);
						
						laser_trans.GlobalPosition = shoot_pos;
						laser_trans.LocalScale = scale;
						
						object[] args = {slave_go, speed, total_damage*overheat, laser_dir, slave_transform.QuatRotation};
						laser_script.CallFunctionArgs("SetInfo", args);
						
						string audio = "Play_shot_2";
						object[] audio_args = {audio};
						
						weapon_manager.CallFunctionArgs("PlayAudio", audio_args);
						
						if(overheat >= 1.0f)
							overheated = true;
						else
						{			
							object[] arg = {overheat};
							weapon_manager.CallFunctionArgs("SetOverheat", arg);
							cooling = true;
						}
						
						charging = false;
						
						TheConsole.Log("FIRE!");
					}
				}
			}
		}
	}
	
}