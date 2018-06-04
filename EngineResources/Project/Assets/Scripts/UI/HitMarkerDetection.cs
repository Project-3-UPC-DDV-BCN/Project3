using TheEngine;
using TheEngine.TheConsole; 

public class HitMarkerDetection 
{

	private TheScript game_manager_script; 
	private TheGameObject slave1;
	private TheScript hitmarker_scpt; 

	void Start () 
	{
		game_manager_script = TheGameObject.Find("GameManager").GetComponent<TheScript>(0);
		
		if(game_manager_script != null)
			slave1 = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");

		if(slave1 != null)
			hitmarker_scpt = slave1.GetComponent<TheScript>(2); 		
	}
	
	void Update () 
	{
		
	}
	
	void OnTriggerEnter(TheCollisionData other)
	{
		if(other.Collider.GetGameObject().tag == "Laser")
		{

			TheScript laser_scpt = other.Collider.GetGameObject().GetComponent<TheScript>(0);
			TheGameObject sender = (TheGameObject)laser_scpt.CallFunctionArgs("GetSender");			

			if(sender.GetComponent<TheTransform>() == slave1.GetComponent<TheTransform>())
			{				 
				hitmarker_scpt.CallFunction("ShowHitMarker");
				TheConsole.Log("ENTEREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEED");  
			}

		}
	}
}