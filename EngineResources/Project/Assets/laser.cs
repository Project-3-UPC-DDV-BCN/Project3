using TheEngine;
using TheEngine.TheConsole;

public class laser {

	TheRigidBody rb;
	TheTransform trans;

	void Start () {
		//rb = TheGameObject.Self.GetComponent<TheRigidBody>();
		//trans = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		TheVector3 vec = TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection*2000*TheTime.DeltaTime;
		TheGameObject.Self.GetComponent<TheRigidBody>().SetLinearVelocity(vec.x, vec.y, vec.z);
		//TheConsole.Log("Laser " + TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection);
	}
}