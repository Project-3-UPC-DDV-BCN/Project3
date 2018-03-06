using TheEngine;
using TheEngine.TheConsole;

public class laser {
	
	public int ti = 200;
	TheRigidBody rb;
	TheTransform trans;

	void Start () {
		//rb = TheGameObject.Self.GetComponent<TheRigidBody>();
		//trans = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		TheGameObject.Self.GetComponent<TheTransform>().GlobalPosition += TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection*ti*TheTime.DeltaTime;
		//TheGameObject.Self.GetComponent<TheRigidBody>().SetLinearVelocity(vec.x + 10, vec.y, vec.z);
		//TheConsole.Log("Laser " + TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection);
	}
	
	void OnCollisionEnter(TheGameObject collided)
	{
		string s = collided.name;
		TheConsole.Log(collided.name);
	}
	
	void OnTriggerEnter(TheGameObject collided)
	{
		TheConsole.Log(collided.name);
	}
}