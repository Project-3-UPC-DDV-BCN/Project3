using TheEngine;
using TheEngine.TheConsole; 

public class HitMarkerDetection 
{

	private TheScript game_manager_script; 

	void Start () 
	{
		game_manager_script = TheGameObject.Find("GameManager").GetComponent<TheScript>(0); 		
	}
	
	void Update () 
	{
		
	}
	
	void OnTriggerEnter(TheCollisionData other)
	{

		TheConsole.Log(other.Collider.GetGameObject().tag); 

		if(other.Collider.GetGameObject().tag == "Laser")
		{
			TheConsole.Log("ENTEREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEED"); 

			TheScript laser_scpt = other.Collider.GetGameObject().GetComponent<TheScript>(0); 
			TheGameObject sender = (TheGameObject)laser_scpt.CallFunctionArgs("GetSender"); 
			TheGameObject slave1 = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");

			if(sender.GetComponent<TheTransform>() == slave1.GetComponent<TheTransform>())
			{
				TheScript hitmarker_scpt = slave1.GetComponent<TheScript>(2); 
				hitmarker_scpt.CallFunction("ShowHitMarker"); 
			}

		}
	}
}