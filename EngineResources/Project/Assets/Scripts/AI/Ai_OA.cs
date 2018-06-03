using TheEngine;
using TheEngine.TheConsole;
using System.Collections.Generic;

public class Ai_OA 
{
	public TheGameObject go_avoidance = null;
	TheTransform go_avoidance_trans = null;
	TheCollider go_avoidance_collider = null;

	public float avoidance_force = 100.0f;

	private bool hitting = false;

	int avoiding_objects = 0;

	void Init () 
	{
		if(go_avoidance != null)
		{
			go_avoidance_trans = go_avoidance.GetComponent<TheTransform>();

			go_avoidance_collider = go_avoidance.GetComponent<TheCollider>();
		}
		else
		{
			//TheConsole.Log("OA: Go is null");
		}
	}
	
	void Update () 
	{
		DoAvoidance();	
	}

	void OnTriggerEnter(TheCollisionData coll) 
	{
		//TheConsole.Log("TriggerEnter");

		if(go_avoidance_collider != coll.Collider)
			++avoiding_objects;
	}

	void OnTriggerExit(TheCollisionData coll) 
	{
        //TheConsole.Log("TriggerExit");

		--avoiding_objects;
	}

	void DoAvoidance() 
	{
		if(go_avoidance_trans != null)
		{	
			if(avoiding_objects > 0)
			{
				TheVector3 avoidance_norm = new TheVector3(1, 1, 0);

				//TheConsole.Log("NewRot: " + avoidance_norm.x + " " + avoidance_norm.x + " " + -avoidance_norm.z);	

				TheVector3 to_add = new TheVector3(-avoidance_norm.x, -avoidance_norm.y, 0);
				go_avoidance_trans.GlobalRotation += to_add * avoidance_force * TheTime.DeltaTime;
			}
		}
	}

}