using TheEngine;
using TheEngine.TheConsole;
using TheEngine.TheMath;

using System.Collections.Generic;

public class Targeting 
{
	private TheGameObject gm_go = null;
	private TheScript gm = null;
	
	public float raycast_distance = 2000.0f;
	public float raycast_x_offset = 0.5f;
	public float raycast_y_offset = 0.5f;
	
	public string controller_front_target_button = "CONTROLLER_Y";
	public string controller_next_target_button = "CONTROLLER_B";
	public string controller_prev_target_button = "CONTROLLER_X";
	public string controller_closest_target_button = "CONTROLLER_A";
	
	public string key_next_target_button = "N";
	public string key_prev_target_button = "P";
	public string key_closest_target_button = "L";

	private TheTransform trans;
	public TheGameObject slavia;
	private TheTransform slavia_trans;
	
	private TheScript target_script = null;
	private TheGameObject target_go = null;
	
	private List<TheGameObject> enemies  = new List<TheGameObject>();
	private int enemy_index = 0;
	
	//UI
	public TheGameObject tie_img;
	public TheGameObject turret_img;
	public TheGameObject generator_img;	
	public TheGameObject target_hp_bar;
	private TheProgressBar bar = null;
	
	void Start () 
	{
		trans = TheGameObject.Self.GetComponent<TheTransform>();
		
		if(slavia != null)
		{
			slavia_trans = slavia.GetComponent<TheTransform>();
		}
		
		gm_go = TheGameObject.Find("GameManager");
		
		if(gm_go != null)
			gm = gm_go.GetScript("GameManager");
		
		if(target_hp_bar != null)
			bar = target_hp_bar.GetComponent<TheProgressBar>();
	}
	
	void Update () 
	{
		//Target front
		if(TheInput.GetControllerButton(0, controller_front_target_button) == 1)
		{
			bool target_found = false;
			//Main ray
			TheConsole.Log("Ray pos: " + trans.GlobalPosition + " dir: " + slavia_trans.ForwardDirection);
			TheRayCastHit[] hits = ThePhysics.RayCastAll(trans.GlobalPosition, slavia_trans.ForwardDirection, raycast_distance);
			
			for(int i = 0; i<hits.Length;++i)
			{
				TheConsole.Log("Ray hit " + i);
				TheGameObject go = hits[i].Collider.GetGameObject();
				if(go != null)
				{
					TheConsole.Log("Ray hit has go");
					TheScript s = go.GetScript("EntityProperties");
				
					if(s != null)
					{
						TheConsole.Log("Ray hit has entity prop");
						
						if((bool)s.CallFunctionArgs("IsShip") || (bool)s.CallFunctionArgs("IsTurret") || (bool)s.CallFunctionArgs("IsGenerator"))
						{
							target_script = s;
							target_go = go;
							target_found = true;
					
							TheConsole.Log("Se Targeteo");									
					
							break;
						}
					}
				}
			}
			
			//Secondary Rays
			if(!target_found)
			{
				for(int ray_x = -1; ray_x < 2 && !target_found; ++ray_x)
				{
					for(int ray_y = -1; ray_y < 2 && !target_found; ++ray_y)
					{
						if(ray_x == 0 && ray_y == 0)
							continue;
						
						TheVector3 ray_inc_xpos = slavia_trans.RightDirection*ray_x*raycast_x_offset;
						TheVector3 ray_inc_ypos = slavia_trans.UpDirection*ray_y*raycast_y_offset;
						TheVector3 ray_pos = trans.GlobalPosition;
						ray_pos.x += ray_inc_xpos.x;
						ray_pos.y += ray_inc_ypos.y;
						TheConsole.Log("Ray pos: " + ray_pos + " dir: " + slavia_trans.ForwardDirection);
						TheRayCastHit[] sec_hits = ThePhysics.RayCastAll(ray_pos, slavia_trans.ForwardDirection, raycast_distance);
						
						for(int i = 0; i<sec_hits.Length;++i)
						{
							TheConsole.Log("Ray hit " + i);
							TheGameObject go = sec_hits[i].Collider.GetGameObject();
							if(go != null)
							{
								TheConsole.Log("Ray hit has go");
								TheScript s = go.GetScript("EntityProperties");
							
								if(s != null)
								{
									TheConsole.Log("Ray hit has entity prop");
									
									if((bool)s.CallFunctionArgs("IsShip") || (bool)s.CallFunctionArgs("IsTurret") || (bool)s.CallFunctionArgs("IsGenerator"))
									{
										target_script = s;
										target_go = go;
										target_found = true;
								
										TheConsole.Log("Se Targeteo");									
								
										break;
									}
								}
							}
						}
					}
				}
			}
		}
		
		//Target Next
		if((TheInput.GetControllerButton(0, controller_next_target_button) == 1 || TheInput.IsKeyDown(key_next_target_button)) && gm != null && target_go != null)
		{
			enemies = (List<TheGameObject>)gm.CallFunctionArgs("GetSlaveEnemies");
			
			for(int i = 0; i<enemies.Count; ++i)
			{
				if(target_go.GetComponent<TheTransform>() == enemies[i].GetComponent<TheTransform>())
				{
					int next = i+1;
					if(next >= enemies.Count)
						next = 0;
					
					target_go = enemies[next];
					target_script = target_go.GetScript("EntityProperties");
					
					TheConsole.Log("Se Targeteo el next!!!");
					
					break;
				}
			}
		}
		
		//Target Previous
		if((TheInput.GetControllerButton(0, controller_prev_target_button) == 1 || TheInput.IsKeyDown(key_prev_target_button)) && gm != null && target_go != null)
		{
			enemies = (List<TheGameObject>)gm.CallFunctionArgs("GetSlaveEnemies");
			
			for(int i = 0; i<enemies.Count; ++i)
			{
				if(target_go.GetComponent<TheTransform>() == enemies[i].GetComponent<TheTransform>())
				{
					int next = i-1;
					if(next < 0)
						next = enemies.Count-1;
					
					target_go = enemies[next];
					target_script = target_go.GetScript("EntityProperties");
					
					TheConsole.Log("Se Targeteo el prev!!!");
					
					break;
				}
			}
		}
		
		//Target Closest enemy
		if((/*TheInput.GetControllerButton(0, controller_closest_target_button) == 1 ||*/ TheInput.IsKeyDown(key_closest_target_button)) && gm != null)
		{
			enemies = (List<TheGameObject>)gm.CallFunctionArgs("GetSlaveEnemies");
			
			if(enemies.Count > 0)
			{
				int closest = 0;
				float closest_no_abs = TheVector3.Distance(enemies[0].GetComponent<TheTransform>().GlobalPosition, slavia_trans.GlobalPosition);
				float closest_dist = TheMath.Abs(closest_no_abs);
				
				for(int i = 1; i<enemies.Count; ++i)
				{
					float curr_dist_no_abs = TheVector3.Distance(enemies[i].GetComponent<TheTransform>().GlobalPosition, slavia_trans.GlobalPosition);
					float curr_dist = TheMath.Abs(curr_dist_no_abs);
					
					if(curr_dist < closest_dist)
						closest = i;
				}
				
				target_go = enemies[closest];
				target_script = target_go.GetScript("EntityProperties");
				
				TheConsole.Log("Se targeteo el closest");
			}
		}
		
		SetUIElements();
	}
	
	TheGameObject GetTarget()
	{
		return target_go;
	}
	
	void SetUIElements()
	{
		if(tie_img != null && turret_img != null && generator_img != null && bar != null)
		{
			if(target_script != null && !(bool)target_script.CallFunctionArgs("IsDead"))
			{
				if((bool)target_script.CallFunctionArgs("IsShip"))
				{
					tie_img.SetActive(true);
					turret_img.SetActive(false);
					generator_img.SetActive(false);
				}
				if((bool)target_script.CallFunctionArgs("IsTurret"))
				{
					tie_img.SetActive(false);
					turret_img.SetActive(true);
					generator_img.SetActive(false);
				}
				if((bool)target_script.CallFunctionArgs("IsGenerator"))
				{
					tie_img.SetActive(false);
					turret_img.SetActive(false);
					generator_img.SetActive(true);
				}
				
				int hp = (int)target_script.CallFunctionArgs("GetLife");
				bar.PercentageProgress = (float)hp;
			}
			else
			{
				bar.PercentageProgress = 0;
				
				tie_img.SetActive(false);
				turret_img.SetActive(false);
				generator_img.SetActive(false);
				
			}
		}
	}
	
	void SetToNull()
	{
		target_go = null;
		target_script = null;
	}
}



