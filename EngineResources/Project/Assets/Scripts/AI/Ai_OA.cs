using TheEngine;
using TheEngine.TheConsole;

public class Ai_OA {

	public TheGameObject parent;
	TheTransform parent_transform;
	TheTransform transform;

	public float Mnv = 10.0f;

	void Start () {
		transform = TheGameObject.Self.GetComponent<TheTransform>();
		if(parent == null)
			parent = TheGameObject.Self.GetParent();
		if(parent != null)
			parent_transform = parent.GetComponent<TheTransform>();
	}
	
	void Update () {
		if(parent == null)
			return;
		transform.LocalPosition = parent_transform.ForwardDirection.Normalized;
		
	}

	void OnTriggerStay(TheGameObject other) {
		if(other == null || parent == null)
			return;
		TheVector3 colDir = other.GetComponent<TheTransform>().GlobalPosition - parent_transform.GlobalPosition;
		TheVector3 newRot = TheVector3.Reflect(-colDir, parent_transform.ForwardDirection);
		float aux = newRot.z;
		newRot.z = newRot.x;
		newRot.x = aux;
		parent_transform.GlobalRotation += newRot.Normalized * Mnv * TheTime.DeltaTime;
		
	}
}