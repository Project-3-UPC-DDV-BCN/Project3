using TheEngine;
using TheEngine.TheConsole;
using TheEngine.TheMath;

using System.Collections.Generic;

public class Targeting 
{
	private TheGameObject gm_go = null;
	private TheScript gm = null;
	
	public float raycast_distance = 2000.0f;
	
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
	}
	
	void Update () 
	{
		//Target front
		if(TheInput.GetControllerButton(0, controller_front_target_button) == 1)
		{
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
					
							TheConsole.Log("Se Targeteo");						
					
							break;
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
				if(target_go.GetComponent<TheTransform>() == enemies[i].GetComponent<TheTransform>() && (i-1) >= 0)
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
		if((TheInput.GetControllerButton(0, controller_closest_target_button) == 1 || TheInput.IsKeyDown(key_closest_target_button)) && gm != null)
		{
			enemies = (List<TheGameObject>)gm.CallFunctionArgs("GetSlaveEnemies");
			
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
	
	TheGameObject GetTarget()
	{
		return target_go;
	}
}