using TheEngine;
using TheEngine.TheConsole;

public class collider_pos_test {

	TheCollider col;
	TheTransform trans;

	void Start () {
		col = TheGameObject.Self.GetComponent<TheCollider>();
		trans = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		TheConsole.Log("Col World: " + col.WorldPosition);
		TheConsole.Log("Col Local: " + col.LocalPosition);
		TheConsole.Log("Trans World: " + trans.GlobalPosition);
	}
}