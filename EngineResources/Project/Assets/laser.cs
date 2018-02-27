using TheEngine;

public class laser {

	TheRigidBody rb;
	TheTransform trans;

	void Start () {
		rb = TheGameObject.Self.GetComponent<TheRigidBody>();
		trans = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		TheVector3 vec = -trans.RightDirection*2000*TheTime.DeltaTime;
		rb.SetLinearVelocity(vec.x, vec.y, vec.z);
	}
}