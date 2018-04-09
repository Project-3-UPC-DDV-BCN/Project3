using TheEngine;
using TheEngine.TheConsole;

public class collision_test {

	void Start () {
		
	}
	
	void Update () {
		
	}
	
	void OnTriggerStay(TheCollisionData col_data)
	{
		TheConsole.Log(col_data.Collider.GetGameObject().name);
		TheConsole.Log(TheGameObject.Self.name);
		TheConsole.Log(col_data.Impulse);
		
		int index = 0;
		foreach(TheContactPoint point in col_data.ContactPoints)
		{
			TheConsole.Log("ContactPoint " + index); 
			TheConsole.Log(point.Position);
			TheConsole.Log(point.Normal);
			TheConsole.Log(point.Separation);
			TheConsole.Log(point.Impulse);
			
			index++;
		}
	}
}