using TheEngine;
using TheEngine.TheConsole;

public class laser {

	TheRigidBody rb;
	TheTransform trans;

	void Start () {
		//rb = TheGameObject.Self.GetComponent<TheRigidBody>();
		trans = TheGameObject.Self.GetComponent<TheTransform>();
		TheConsole.Log(TheRandom.RandomInt());
		TheConsole.Log(TheRandom.RandomFloat());
		TheConsole.Log(TheRandom.RandomRange(5,20));
		TheConsole.Log(TheGameObject.Find("Main Camera").name);
	}
	
	void Update () {
		TheVector3 vec = TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection*2000*TheTime.DeltaTime;
		//TheGameObject.Self.GetComponent<TheRigidBody>().SetLinearVelocity(vec.x, vec.y, vec.z);
		//TheConsole.Log("Laser " + TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection);
		//TheConsole.Log(trans);
		//TheConsole.Log(vec);
	}
}