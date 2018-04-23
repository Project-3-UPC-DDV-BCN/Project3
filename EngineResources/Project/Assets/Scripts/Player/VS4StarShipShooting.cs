using TheEngine;	
using TheEngine.TheConsole;

using TheEngine.TheMath;

public class VS4StarShipShooting 
{

	TheFactory laser_factory;
	
	public TheGameObject laser_spawner;
	
	// Audio
	public TheGameObject shoot_audio_emiter;
	TheAudioSource audio_source;
	
	// UI
	public TheGameObject overheat_bar_obj;
	TheProgressBar overheat_bar_bar = null;
	
	public TheGameObject crosshair_1;
	public TheGameObject crosshair_2;
	
	public TheGameObject weapon_icon_1;
	public TheGameObject weapon_icon_2;

	public TheGameObject weapons_energy;
    TheProgressBar weapons_bar = null;
	
	// Particles
	public TheGameObject laser_light;
	TheLight laser_light_comp;
	
	// Timers and durations
	public float spawn_time = 0.01f;
	float timer = 0.1f;
	float light_duration = 0.2f;
	
	public float overheat_time = 2.0f;
    public float overheated_time = 3.0f;
    float overheat_timer = 0.0f;
	
	// Booleans	
	bool light_on = false;
	bool used_left_laser = false;
	bool overheated = false;
	bool cooling = false;
   
   	// Slave 1 Properties
    public int num_weapons = 2;
    public int weapon = 0;
	
	public float overheat_increment = 0.2f;
    float curr_overheat_inc;
    float overheat = 0.0f; //from 0.0 to 1.0
    
    public float w1_cooling_rate = 10.0f;
    public float w2_cooling_rate = 10.0f;
    
	private TheScript weapon_script = null;

	private TheTimer shooting_frequency_timer = new TheTimer();
	public float shooting_frequency_w0 = 0.2f;
	public float shooting_frequency_w1 = 0.2f;
	
	float shooting_frequency_w0_base;
	
    void Start () 
	{
		laser_factory = TheGameObject.Self.GetComponent<TheFactory>();
		
		if(laser_factory != null)
        	laser_factory.StartFactory();

		audio_source = shoot_audio_emiter.GetComponent<TheAudioSource>();
		/*if(audio_source == null) 
			TheConsole.Log("no audio");*/

        weapons_bar = weapons_energy.GetComponent<TheProgressBar>();
		/*if(weapons_bar == null) 
			TheConsole.Log("no weapon bar");*/

        curr_overheat_inc = overheat_increment;
	
		if(overheat_bar_obj != null)
			overheat_bar_bar = overheat_bar_obj.GetComponent<TheProgressBar>();

		/*if(overheat_bar_bar == null) 
			TheConsole.Log("no overheat");*/

		if(crosshair_1 != null)
			crosshair_1.SetActive(true);

		if(crosshair_2 != null)
			crosshair_2.SetActive(false);

		if(weapon_icon_1 != null)
			weapon_icon_2.SetActive(true);
		
		if(weapon_icon_2 != null)
			weapon_icon_2.SetActive(false);
		
		if(laser_light != null)
			laser_light_comp = laser_light.GetComponent<TheLight>();
		
		weapon_script = TheGameObject.Self.GetScript("Weapon0");

		shooting_frequency_timer.Start();
		
		shooting_frequency_w0_base = shooting_frequency_w0;
	}	

	void Update () 
	{
		if(overheat_bar_bar != null)
			overheat_bar_bar.PercentageProgress = overheat * 100;
		
		switch (weapon)
		{
			case 0:
			{	
				if(timer <= 0)
				{
					float percent = weapons_bar.PercentageProgress;
					
					shooting_frequency_w0 = -75.16412f + (0.2187436f + 75.16412f)/(1 + TheMath.Pow(percent/381393900, 0.4224618f));
					
					/*if (percent > 45.0 && percent < 55.0)
						
					else if (percent >= 55.0)
						shooting_frequency_w0 = shooting_frequency_w0_base + shooting_frequency_w0_base * percent / 100;
					else if (percent <= 45.0)
						shooting_frequency_w0 = shooting_frequency_w0_base - (100 - shooting_frequency_w0_base) * percent / 100;
					*/
					if(TheInput.GetControllerButton(0,"CONTROLLER_A") == 2 && shooting_frequency_timer.ReadTime() > shooting_frequency_w0)
					{
						if (weapon_script == null && weapons_bar == null && laser_factory == null && laser_spawner == null)
							break;
						
						// SHOOT
						object[] args_shoot = {weapons_bar, curr_overheat_inc, overheat_increment, used_left_laser, laser_factory, laser_spawner, audio_source};		
						weapon_script.CallFunctionArgs("Shoot", args_shoot);		
																	
						// EDIT LIGHT COMPONENT
						object[] args_light = {laser_light_comp};
						weapon_script.CallFunctionArgs("EditLightComp", args_light);						
						
						timer = spawn_time;
						
						// OVERHEAT
						//object[] args_heat = {overheat, curr_overheat_inc, overheated, overheat_timer, overheat_time, overheated_time};
						//weapon_script.CallFunctionArgs("Heat", args_heat);
						
						shooting_frequency_timer.Start();						
					}
					else if (TheInput.GetControllerButton(0, "CONTROLLER_A") == 3)
					{
						object[] args_overheat = {overheat, cooling, laser_factory, laser_spawner, overheat_timer};
						weapon_script.CallFunctionArgs("Overheat", args_overheat);
					}
				}
				
				object[] args_cooling = {overheat_timer, overheat, w1_cooling_rate};
				weapon_script.CallFunctionArgs("Cooling", args_cooling);

			
				break;
			}
			case 1:
			{
				if (weapon_script == null && weapons_bar == null && laser_factory == null && laser_spawner == null)
					break;

				if(TheInput.GetControllerButton(0,"CONTROLLER_A") == 2)
				{
					if (!cooling)
					{
						overheat += curr_overheat_inc;
						overheat_timer = 1.0f;
					}				
				}
				else if (TheInput.GetControllerButton(0,"CONTROLLER_A") == 3 && !cooling)
				{
					// SHOOT
					object[] args_shoot = {weapons_bar, curr_overheat_inc, overheat_increment, laser_factory, laser_spawner, audio_source};		
					weapon_script.CallFunctionArgs("Shoot", args_shoot);
				}

				object[] args_cooling = {overheat_timer, overheat, w1_cooling_rate};
				weapon_script.CallFunctionArgs("Cooling", args_cooling);
				
				break;
			}
		}			
   
		if (overheat <= 0.0f)	
			overheated = false;
   
		timer -= TheTime.DeltaTime;
		overheat_timer -= TheTime.DeltaTime;
		
		light_duration -= TheTime.DeltaTime;
		
		if (light_duration <= 0.0f && laser_light != null && light_on == true)
		{
			light_on = false;
			laser_light.GetComponent<TheLight>().SetComponentActive(false);
		}

        if(TheInput.IsKeyDown("C"))
			ChangeWeapon();
    }

    void ChangeWeapon()
	{
		if(audio_source != null)
			audio_source.Play("Play_change_weapon");

		weapon++;
		weapon %= num_weapons;

		TheVector3 size = new TheVector3(1, 1, 1);

		if(crosshair_1 != null && crosshair_2 != null && crosshair_1.IsActive())
		{
			crosshair_1.SetActive(false);
			crosshair_2.SetActive(true);
						
			if(weapon_icon_1 != null)
				weapon_icon_1.SetActive(false);

			if(weapon_icon_2 != null)
				weapon_icon_2.SetActive(true);
		}
		else if(crosshair_1 != null && crosshair_2 != null)
		{
			crosshair_1.SetActive(true);
			crosshair_2.SetActive(false);

			if(weapon_icon_1 != null)
				weapon_icon_1.SetActive(true);

			if(weapon_icon_2 != null)
				weapon_icon_2.SetActive(false);
		}
		
		switch (weapon) 
		{
			case 0:
				weapon_script = TheGameObject.Self.GetScript("Weapon0");
				break;
			case 1:
				weapon_script = TheGameObject.Self.GetScript("Weapon1");
				break;
		}
	}
}