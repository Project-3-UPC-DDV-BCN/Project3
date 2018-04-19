using TheEngine;

using TheEngine.TheConsole;
using TheEngine.TheMath;


public class StarShipShooting {

	TheFactory laser_factory;
	
	// Audio
    public TheGameObject laser_spawner;
	public TheGameObject audio_emiter;
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
    
	TheScript weapon_script = null;
	
    void Start () 
	{
		laser_factory = TheGameObject.Self.GetComponent<TheFactory>();
		
		if(laser_factory != null)
        	laser_factory.StartFactory();

		audio_source = audio_emiter.GetComponent<TheAudioSource>();
		if(audio_source == null) 
			TheConsole.Log("no audio");

        weapons_bar = weapons_energy.GetComponent<TheProgressBar>();
		if(weapons_bar == null) 
			TheConsole.Log("no weapon bar");

        curr_overheat_inc = overheat_increment;
	
		if(overheat_bar_obj != null)
			overheat_bar_bar = overheat_bar_obj.GetComponent<TheProgressBar>();

		if(overheat_bar_bar == null) 
			TheConsole.Log("no overheat");

		if(crosshair_1 != null)
			crosshair_1.SetActive(true);

		if(crosshair_2 != null)
			crosshair_2.SetActive(false);

		if(weapon_icon_1 != null)
			weapon_icon_2.SetActive(true);
		
		if(weapon_icon_2 != null)
			weapon_icon_2.SetActive(false);
		
		weapon_script = TheGameObject.GetScript("VS4 - Weapon0");
    }	

	void Update () 
	{
		if(overheat_bar_bar != null)
			overheat_bar_bar.PercentageProgress = overheat * 100;

		if(timer <= 0 && !overheated)
		{
			if(TheInput.GetControllerButton(0,"CONTROLLER_A") == 2)
			{
                switch (weapon)
                {
                	case 0:
                        {
							if (weapons_bar == null && laser_factory == null && laser_spawner == null)
								break;
							
							weapon_script.Shoot(weapons_bar, curr_overheat_inc, overheat_increment, used_left_laser, laser_factory, laser_spawner);		
							
							if(audio_source != null)
                            	audio_source.Play("Play_shot");
							

                            timer = spawn_time;
							
							
							

							if (laser_light != null){
								laser_light.GetComponent<TheLight>().SetComponentActive(true);
								light_duration = 0.2f;
								light_on = true;
							}
							
                            //Add heat
                            overheat += curr_overheat_inc;
                            if (overheat >= 1.0f)
                            {
                                overheated = true;
                                overheat_timer = overheated_time;
                            }
                            else 
								overheat_timer = overheat_time;              
							break;
                        }
                    case 1:
                        {
                            if (!cooling)
                            {
                                overheat += curr_overheat_inc;
                                overheat_timer = 1.0f;
                            }
                            break;
                        }
                }
            }

            if (TheInput.GetControllerButton(0, "CONTROLLER_A") == 1)
            {
                if(overheat > 0.0f && !cooling)
                {
                    TheVector3 offset = new TheVector3(0, 2, 0);
					
					if(laser_factory != null && laser_spawner != null)
					{
                    	laser_factory.SetSpawnPosition(laser_spawner.GetComponent<TheTransform>().GlobalPosition + offset);
	
                   		TheVector3 vec = laser_spawner.GetComponent<TheTransform>().ForwardDirection * 20000 * TheTime.DeltaTime;

                    	TheVector3 size = new TheVector3(1 + overheat, 1 + overheat, 1 + overheat);

                   		laser_factory.SetSpawnScale(size);

                    	TheGameObject go = laser_factory.Spawn();

                    	go.GetComponent<TheRigidBody>().SetLinearVelocity(vec.x, vec.y, vec.z);
					}
					
                    overheat_timer = 0.0f;
                    cooling = true;
                }
            }


        }

		else if(!overheated)
		{
			timer -= TheTime.DeltaTime;
		}

        if(overheat_timer<=0.0f)
        {
            //start cooling
            switch (weapon)
            {
                case 0:
                    overheat -= w1_cooling_rate * TheTime.DeltaTime; ;
                    break;
                case 1:
                    overheat -= w2_cooling_rate * TheTime.DeltaTime; ;
                    break;
            }
            
            if(overheat<=0.0f)
            {
                overheat = 0.0f;
                overheated = false;
                cooling = false;
            }
        }
		
		light_duration -= TheTime.DeltaTime;
		
		if (light_duration <= 0.0f && laser_light != null && light_on == true)
		{
			light_on = false;
			laser_light.GetComponent<TheLight>().SetComponentActive(false);
		}
        overheat_timer -= TheTime.DeltaTime;

        if(TheInput.IsKeyDown("C"))
        {
			audio_source.Play("Play_change_weapon");
            weapon++;
            weapon %= num_weapons;

            TheVector3 size = new TheVector3(1, 1, 1);
			
			if(laser_factory != null)
            	laser_factory.SetSpawnScale(size);
			
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
        }
    }

    
}