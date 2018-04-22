using TheEngine;

using TheEngine.TheConsole;
using TheEngine.TheMath;

public class Weapon0 
{	
	public int damage = 10;
	public float speed = 100;

	private TheGameObject slave_go = null;
	private TheTransform slave_transform = null;

	private TheScript starship_shooting = null;
	
	private bool cool = false;
	TheTimer heat_timer = new TheTimer();
	public float heat_time = 2.0f;
	
	void Start()
	{
		starship_shooting = TheGameObject.Self.GetScript("VS4StarShipShooting");
		
		TheGameObject game_manager = TheGameObject.Find("GameManager");
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
	
	public void Shoot(TheProgressBar weapons_bar, float curr_overheat_inc, float overheat_increment, bool used_left_laser, 
				TheFactory laser_factory, TheGameObject laser_spawner, TheAudioSource audio_source)
	{
		/*if (weapons_bar == null)
			TheConsole.Log("weapons_bar == null");
		if (curr_overheat_inc == null)
			TheConsole.Log("curr_overheat_inc == null");
		if (overheat_increment == null)
			TheConsole.Log("overheat_increment == null");
		if (used_left_laser == null)
			TheConsole.Log("used_left_laser == null");
		if (laser_factory == null)
			TheConsole.Log("laser_factory == null");
		if (laser_spawner == null)
			TheConsole.Log("laser_spawner == null");
		if (audio_source == null)
			TheConsole.Log("audio_source == null");
		*/
		
		TheVector3 offset;
	
		starship_shooting.SetFloatField("curr_overheat_inc", overheat_increment * 1.5f - overheat_increment * (weapons_bar.PercentageProgress / 100.0f));

		if (used_left_laser)
		{
			offset = new TheVector3(1, 2, 0);
		}
		else
		{
			offset = new TheVector3(-1, 2, 0);
		}

		starship_shooting.SetBoolField("used_left_laser", !used_left_laser);

		//laser_factory.SetSpawnPosition(laser_spawner.GetComponent<TheTransform>().GlobalPosition);// + offset
		if(laser_factory != null)
		{
			TheGameObject go = laser_factory.Spawn();

			if(go != null)
			{
				TheScript laser_script = go.GetScript("Laser"); 
				if(laser_script != null && slave_transform != null && slave_go != null)
				{
					//TheConsole.Log("Slave1 shoots with weapon 0");
					//TheConsole.Log(slave_transform.ForwardDirection.x + " " + slave_transform.ForwardDirection.y + " " + slave_transform.ForwardDirection.z);
					object[] args = {slave_go, speed, damage, slave_transform.ForwardDirection, slave_transform.QuatRotation};
					laser_script.CallFunctionArgs("SetInfo", args);
				}
			}
		}
		
		if(audio_source != null)
		{	
			audio_source.Stop("Play_shot");
			audio_source.Play("Play_shot");
		}
	}

	public void EditLightComp(TheLight laser_light_comp)
	{
		if (laser_light_comp != null){
			laser_light_comp.SetComponentActive(true);
			starship_shooting.SetFloatField("light_duration", 0.2f);
			starship_shooting.SetBoolField("light_on", true);
		}
	}
	
	public void Heat(float overheat, float curr_overheat_inc, bool overheated, float overheat_timer, float overheat_time, float overheated_time)
	{	
		overheat += curr_overheat_inc;
		starship_shooting.SetFloatField("overheat", overheat);
		
		if (overheat >= 1.0f)
		{
			starship_shooting.SetBoolField("overheated", true);
			starship_shooting.SetFloatField("overheat_timer", overheated_time);
		}
		else 
			starship_shooting.SetFloatField("overheat_timer", overheat_time); 
		
		heat_timer.Stop();
		starship_shooting.SetBoolField("cooling", false);
	}
	
	public void Overheat(float overheat, bool cooling, TheFactory laser_factory, TheGameObject laser_spawner, float overheat_timer)
	{
		if(overheat > 0.0f && !cooling)
		{
			TheVector3 offset = new TheVector3(0, 2, 0);
			
			starship_shooting.SetFloatField("overheat_timer", 0.0f);
			starship_shooting.SetBoolField("cooling", true);		
		}
	}
    
	public void Cooling(float overheat_timer, float overheat, float w1_cooling_rate)
	{
		if (overheat >= 1.0f)
			cool = true;		
		else if (overheat > 0.0f && heat_timer.ReadTime() <= 0.0f)
			heat_timer.Start();
		
		if (cool)
		{
			if(overheat_timer <= 0.0f)
			{
				overheat -= w1_cooling_rate * TheTime.DeltaTime;
				
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
				overheat -= w1_cooling_rate * TheTime.DeltaTime;
				
				if(overheat<=0.0f)
				{
					//TheConsole.Log("Stopped");
					overheat = 0.0f;
					heat_timer.Stop();
				}
				
				starship_shooting.SetFloatField("overheat", overheat);
			}
		}
	}
}