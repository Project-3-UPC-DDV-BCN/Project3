using TheEngine;
using TheEngine.TheConsole;

public class test2 {
	
	public TheGameObject target;
	TheTransform self_t;
	TheTransform target_t;
	
	//Start method
	void Start () {
		self_t = TheGameObject.Self.GetComponent<TheTransform>();
		target_t = target.GetComponent<TheTransform>();
	}
	
	void Update () {
		self_t.LookAt(target_t.GlobalPosition);
		self_t.GlobalPosition += self_t.ForwardDirection * TheTime.DeltaTime * 20;
	}
}