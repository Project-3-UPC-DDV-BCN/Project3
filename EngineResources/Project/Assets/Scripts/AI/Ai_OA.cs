using TheEngine;
using TheEngine.TheConsole;
using System.Collections.Generic;

public class Ai_OA 
{
	public TheGameObject go_avoidance = null;
	TheTransform go_avoidance_trans = null;

	public float avoidance_force = 100.0f;

	private bool hitting = false;

	private List<TheCollider> avoiding_objects = new List<TheCollider>();

	void Init () 
	{
		if(go_avoidance != null)
		{
			go_avoidance_trans = go_avoidance.GetComponent<TheTransform>();

			if(go_avoidance_trans == null)
			{
				//TheConsole.Log("OA: Go trans is null");
			}
			else
			{
				//TheConsole.Log("OA: Go trans is null not null!!");
			}
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
		TheConsole.Log("TriggerEnter");

		TheGameObject other_ship = coll.Collider.GetGameObject();

		if(other_ship != go_avoidance)
			avoiding_objects.Add(coll.Collider);
	}

	void OnTriggerExit(TheCollisionData coll) 
	{
        //TheConsole.Log("TriggerExit");

		TheGameObject other_ship = coll.Collider.GetGameObject();
		avoiding_objects.Remove(coll.Collider);
	}

	void DoAvoidance() 
	{
		if(go_avoidance_trans != null)
		{
			TheVector3 avoidance = new TheVector3(0, 0, 0);
		
			for(int i = 0; i < avoiding_objects.Count; ++i)
			{
				TheVector3 colDir = go_avoidance_trans.GlobalPosition - avoiding_objects[i].WorldPosition;
				avoidance += colDir;
			}
			
			TheVector3 avoidance_norm = new TheVector3(0, 0, 0);
			avoidance_norm = TheVector3.Normalize(avoidance);

			//TheVector3 newRot = TheVector3.Reflect(avoidance_norm, go_avoidance_trans.ForwardDirection);

			TheConsole.Log("NewRot: " + -avoidance_norm.x + " " + -avoidance_norm.x + " " + -avoidance_norm.z);

			TheVector3 to_add = new TheVector3(-avoidance_norm.x, -avoidance_norm.y, 0);
			go_avoidance_trans.GlobalRotation += to_add * avoidance_force * TheTime.DeltaTime;
		}
	}

}