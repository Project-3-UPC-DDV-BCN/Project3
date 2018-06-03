using TheEngine;
using TheEngine.TheConsole;

public class PlayerWeaponManager 
{
	//Shoot button
	public bool is_joystick = true;
	public int joystick_sensivility = 200;
	public string shoot_button = "RIGHT_TRIGGER";
	private bool was_pressed = false;
	
	//Curr weapon script
	private TheScript weapon_script = null;
	
	//Weapons info
	public int num_weapons = 2;
	private int curr_weapon = 0;
	public string p1_change_weapon = "CONTROLLER_RB";
	public string p2_change_weapon = "C";
	private float overheat = 0.0f;
	public string weapon0_script_name = "BaseLaser";
	public string weapon1_script_name = "ChargeLaser";
	private bool can_shoot = true;	
	///weapons bar
	public TheGameObject weapons_energy;
    TheProgressBar weapons_bar = null;

	//Overheat bar
	public TheGameObject overheat_bar_obj;
	private TheProgressBar overheat_bar = null;
	
	//Crosshair
	public TheGameObject crosshair_1;
	public TheGameObject crosshair_2;
	
	//Weapon Icons
	public TheGameObject weapon_icon_1;
	public TheGameObject weapon_icon_2;
	
	//Laser light
	public TheGameObject laser_light;
	private TheLight laser_light_comp;
	private bool light_on = false;
	private float light_duration = 0.0f;
	
	//Factory
	TheFactory laser_factory = null;
	
	//Audio
	public TheGameObject shoot_audio_emiter;
	TheAudioSource audio_source;
	
	void Start () 
	{
		if(weapon_script == null)
			weapon_script = TheGameObject.Self.GetScript(weapon0_script_name);
		
		if(overheat_bar_obj != null)
			overheat_bar = overheat_bar_obj.GetComponent<TheProgressBar>();
		
		if(laser_light != null)
			laser_light_comp = laser_light.GetComponent<TheLight>();
		
		//Set initial crosshair
		if(crosshair_1 != null)
			crosshair_1.SetActive(true);

		if(crosshair_2 != null)
			crosshair_2.SetActive(false);

		if(weapon_icon_1 != null)
			weapon_icon_2.SetActive(true);
		
		if(weapon_icon_2 != null)
			weapon_icon_2.SetActive(false);
		
		if(overheat_bar_obj != null)
			overheat_bar_obj.SetActive(false);
		
		//Get weapons energy bar
		if(weapons_energy != null)
			weapons_bar = weapons_energy.GetComponent<TheProgressBar>();
		
		laser_factory = TheGameObject.Self.GetComponent<TheFactory>();
		
		//Get factory
		if(laser_factory != null)
        	laser_factory.StartFactory();
		
		//Get audio_source
		if(shoot_audio_emiter != null)
			audio_source = shoot_audio_emiter.GetComponent<TheAudioSource>();
		
	}
	
	void Update () 
	{
		if(can_shoot) // Perform curr_weapon shoot
		{
			float energy_percentage = weapons_bar.PercentageProgress;
			object[] args = {energy_percentage};
			
			if(is_joystick)
			{
				if(TheInput.GetControllerJoystickMove(0, shoot_button) > joystick_sensivility)
				{
					was_pressed = true;
					
					overheat = (float)weapon_script.CallFunctionArgs("ShootPress", args);
				}
				
				else if(was_pressed)
				{
					was_pressed = false;
					
					weapon_script.CallFunctionArgs("ShootRelease");
				}
			}
			else
			{
				if(TheInput.GetControllerButton(0, shoot_button) == 2)
				{
					overheat = (float)weapon_script.CallFunctionArgs("ShootPress", args);
				}
				
				else if(TheInput.GetControllerButton(0, shoot_button) == 3)
				{
					weapon_script.CallFunctionArgs("ShootRelease");
				}
			}
			
			//Change weapon
			if(TheInput.GetControllerButton(0, p1_change_weapon) == 1 || TheInput.IsKeyDown(p2_change_weapon))
			{
				ChangeWeapon();
			}
		}
		
		//set overheat bar value
		if(overheat_bar != null)
			overheat_bar.PercentageProgress = overheat * 100;
		
		//check laser light state
		if(laser_light_comp != null && light_duration <= 0.0f && light_on)
		{
			laser_light_comp.SetComponentActive(false);
			light_on = false;
		}
		else if(laser_light_comp != null)
			light_duration -= TheTime.DeltaTime;
	}
	
	void TurnOnLaserLight(float duration)
	{
		if(laser_light_comp != null)
		{
			laser_light_comp.SetComponentActive(true);
			light_on = true;
			light_duration = duration;
		}
	}
	
	void SetCanShoot(bool set)
	{
		can_shoot = set;
	}
	
	TheFactory GetFactory()
	{
		return laser_factory;
	}
	
	void PlayAudio(string audio)
	{
		if(audio_source != null)
		{	
			audio_source.Stop(audio);
			audio_source.Play(audio);
		}
	}
	
	void ChangeWeapon()
	{
		if(audio_source != null)
			{
				audio_source.Play("Play_change_weapon");
				audio_source.Play("Stop_Charge");

			}

		curr_weapon++;
		curr_weapon %= num_weapons;
		
		if(crosshair_1 != null && crosshair_2 != null && crosshair_1.IsActive())
		{
			crosshair_1.SetActive(false);
			crosshair_2.SetActive(true);
			overheat_bar_obj.SetActive(true);
						
			if(weapon_icon_1 != null)
				weapon_icon_1.SetActive(false);

			if(weapon_icon_2 != null)
				weapon_icon_2.SetActive(true);
		}
		else if(crosshair_1 != null && crosshair_2 != null)
		{
			crosshair_1.SetActive(true);
			crosshair_2.SetActive(false);
			overheat_bar_obj.SetActive(false);

			if(weapon_icon_1 != null)
				weapon_icon_1.SetActive(true);

			if(weapon_icon_2 != null)
				weapon_icon_2.SetActive(false);
		}
		
		switch (curr_weapon)
		{
			case 0:
				weapon_script = TheGameObject.Self.GetScript(weapon0_script_name);
				break;
			case 1:
				weapon_script = TheGameObject.Self.GetScript(weapon1_script_name);
				break;
		}
	}
	
	void SetOverheat(float val)
	{
		overheat = val;
	}
}