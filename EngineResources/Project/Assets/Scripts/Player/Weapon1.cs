using TheEngine;

using TheEngine.TheConsole;
using TheEngine.TheMath;


public class Weapon1 
{
	public int damage = 10;
	public float speed = 100;

	private TheGameObject slave_go = null;
	private TheTransform slave_transform = null;
	
	private TheScript starship_shooting = null;

	private bool cool = false;
	TheTimer heat_timer = new TheTimer();
	public float heat_time = 2.0f;
	
	TheTimer cooldown_timer = new TheTimer();
	public float cooldown_timer = 2.0f;
	
	private TheGameObject game_manager = null;
	
	void Init()	
	{
		game_manager = TheGameObject.Find("GameManager");

		starship_shooting = TheGameObject.Self.GetScript("VS4StarShipShooting");
	}

	void Start()
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
	}

	public void Shoot(TheProgressBar weapons_bar, float curr_overheat_inc, float overheat_increment, TheFactory laser_factory, 
					TheGameObject laser_spawner, TheAudioSource audio_source)
	{
		/*if (weapons_bar == null)
			TheConsole.Log("weapons_bar == null");
		if (curr_overheat_inc == null)
			TheConsole.Log("curr_overheat_inc == null");
		if (overheat_increment == null)
			TheConsole.Log("overheat_increment == null");
		if (laser_factory == null)
			TheConsole.Log("laser_factory == null");
		if (laser_spawner == null)
			TheConsole.Log("laser_spawner == null");
		if (audio_source == null)
			TheConsole.Log("audio_source == null");
		*/
		
		TheVector3 offset = new TheVector3(0, 2, 0);

		//starship_shooting.SetFloatField("curr_overheat_inc", overheat_increment * 1.5f - overheat_increment * (weapons_bar.PercentageProgress / 100.0f));
		
		if(laser_factory != null)
		{
			TheGameObject go = laser_factory.Spawn();

			if(go != null)
			{
				TheScript laser_script = go.GetScript("Laser"); 
				if(laser_script != null && slave_transform != null && slave_go != null)
				{
					//TheConsole.Log("Slave1 shoots with weapon 1");			
	
					object[] args = {slave_go, speed, damage, slave_transform.ForwardDirection, slave_transform.QuatRotation};
					laser_script.CallFunctionArgs("SetInfo", args);
				}
			}
		}

		if(audio_source != null)
		{	
			audio_source.Stop("Play_shot_2");
			audio_source.Play("Play_shot_2");
		}
		
		starship_shooting.SetBoolField("cooling", true);
		heat_timer.Stop();
	}
	
	void Cooling(float overheat_timer, float overheat, float w2_cooling_rate)
	{
		if (overheat >= 1.0f)
			cool = true;		
		else if (overheat > 0.0f && heat_timer.ReadTime() <= 0.0f)
			heat_timer.Start();
		
		if (overheat <= 0.0f)
			starship_shooting.SetBoolField("cooling", false);
		
		if (cool)
		{
			if(overheat_timer <= 0.0f)
			{
				overheat -= w2_cooling_rate * TheTime.DeltaTime;
				
				if(overheat<=0.0f)
				{
					overheat = 0.0f;
					starship_shooting.SetBoolField("overheated", false);
					starship_shooting.SetBoolField("cooling", false);
					cool = false;
				}
				
				starship_shooting.SetFloatField("overheat", overheat);
			}
		}
		else
		{
			if (heat_timer.ReadTime() >= heat_time)
			{
				overheat -= w2_cooling_rate * TheTime.DeltaTime;
				
				if(overheat<=0.0f)
				{
					//TheConsole.Log("Stopped");
					overheat = 0.0f;
					starship_shooting.SetBoolField("cooling", false);
					heat_timer.Stop();
				}
				
				starship_shooting.SetFloatField("overheat", overheat);
			}
		}

		
	}
}