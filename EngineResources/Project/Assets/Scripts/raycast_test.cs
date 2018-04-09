using TheEngine;
using TheEngine.TheConsole;

public class raycast_test {

	void Start () {
		
	}
	
	void Update () {
		if(TheInput.IsKeyDown("Z"))
		{
			
			TheRayCastHit[] hits = ThePhysics.RayCastAll(new TheVector3(0,0,0), new TheVector3(0,0,1), 200);
			foreach(TheRayCastHit hit in hits)
			{
				if(hit != null)
				{
					TheConsole.Log(hit);
					TheConsole.Log(hit.ContactPoint);
					TheConsole.Log(hit.ContactPoint.z);
					TheConsole.Log(hit.Normal);
					TheConsole.Log(hit.Distance);
					TheConsole.Log(hit.Collider);
				}
			}
		}
	}
}