using TheEngine;

public class Missile {

	int speed = 30;
	
	void Start () {
		TheGameObject go = null;
		go.SetActive(true);
	}
	
	void Update () {
		if(TheGameObject.Self.IsActive())
        {
            TheGameObject.Self.GetComponent<TheTransform>().GlobalPosition += TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection * Time.DeltaTime * speed;
        }
	}
}