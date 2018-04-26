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
	public string p1_change_weapon = "CONTROLLER_RB"
	public string p2_change_weapon = "C"
	private float overheat = 0.0f;

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
	
	void Start () 
	{
		if(curr_weapon == null)
			weapon_script = TheGameObject.Self.GetScript("Weapon0");
		
		if(overheat_bar_obj != null)
			overheat_bar = overheat_bar_obj.GetComponent<TheProgressBar>();
		
		if(laser_light != null)
			laser_light_comp = laser_light.GetComponent<TheLight>();
	}
	
	void Update () 
	{
		if(is_joystick)
		{
			if(TheInput.GetControllerJoystickMove(0, shoot_button) > joystick_sensivility)
			{
				was_pressed = true;
				
				overheat = 
			}
			
			else if(was_pressed)
			{
				was_pressed = false;
				
			}
		}
		else
		{
			if(TheInput.GetControllerButton(0, shoot_button) == 2)
			{
				
			}
			
			else if(TheInput.GetControllerButton(0, shoot_button) == 3)
			{
				
			}
		}
		
		if(overheat_bar != null)
			overheat_bar_bar.PercentageProgress = overheat * 100;
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
}