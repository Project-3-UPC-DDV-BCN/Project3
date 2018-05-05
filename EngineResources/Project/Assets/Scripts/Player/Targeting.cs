using TheEngine;
using TheEngine.TheConsole;

public class Targeting 
{
	
	public float raycast_distance = 100.0f;
	
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
	
	void Start () 
	{
		trans = TheGameObject.Self.GetComponent<TheTransform>();
		
		if(slavia != null)
		{
			slavia_trans = slavia.GetComponent<TheTransform>();
		}
	}
	
	void Update () 
	{
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
						
						if((bool)s.CallFunctionArgs("IsShip"))
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
	}
}