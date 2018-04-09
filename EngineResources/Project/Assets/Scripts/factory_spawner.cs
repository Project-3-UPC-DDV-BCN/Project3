using TheEngine;

public class factory_spawner {

	TheFactory factory;

	void Start () {
		factory = TheGameObject.Self.GetComponent<TheFactory>();
		factory.StartFactory();
	}
	
	void Update () {
		if(TheInput.IsKeyDown("Z"))
		{
			TheGameObject go = factory.Spawn();
			TheRigidBody rb = go.GetComponent<TheRigidBody>();
			rb.SetLinearVelocity(0,0,100);
		}
	}
}