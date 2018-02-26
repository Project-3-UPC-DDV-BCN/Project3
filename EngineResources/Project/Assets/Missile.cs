using TheEngine;
using TheEngine.TheConsole;

public class Missile {

	int speed = 10;
	//TheTransform laser_transform;
	
	void Start () {
		//laser_transform = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		if(TheGameObject.Self.IsActive())
        {
            TheGameObject.Self.GetComponent<TheTransform>().GlobalPosition += -TheGameObject.Self.GetComponent<TheTransform>().RightDirection * Time.DeltaTime * speed;
        	TheConsole.Log(TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection);
		}
	}
}