using TheEngine;
using TheEngine.Math;

public class Tank_Turret {

	public int rotation_speed = 30;
	
	void Start () {
		
	}
	
	void Update () {
		float new_rotation_y = TheGameObject.Self.GetComponent<TheTransform>().LocalRotation.y + TheInput.GetMouseXMotion() * Time.DeltaTime * rotation_speed;
		new_rotation_y = Math.Clamp(new_rotation_y, 70, -70);
		TheGameObject.Self.GetComponent<TheTransform>().LocalRotation = new TheVector3(0,new_rotation_y,0);
	}
}