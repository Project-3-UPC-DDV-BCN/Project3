using TheEngine;
using TheEngine.TheConsole;

public class Ai_OA {

	public TheGameObject parent;
	TheTransform parent_transform;
	TheTransform transform;

	public float Mnv = 10.0f;

	private bool hitting = false;

	private TheGameObject obstacleObject = null;

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

		if(hitting == true && obstacleObject != null) {
			ObstacleAvoidance(obstacleObject);
		}		

	}

	void OnTriggerEnter(TheGameObject other) {
		//TheConsole.Log("TriggerEnter");
		hitting = true;
		obstacleObject = other;
	}
	void OnTriggerExit(TheGameObject other) {
        //TheConsole.Log("TriggerExit");
		if(other == obstacleObject) {
			hitting = false;
			obstacleObject = null;
		}
	}

	void ObstacleAvoidance(TheGameObject other) 
	{
		if(other == null || parent == null)
			return;
        TheTransform otherTrans = other.GetComponent<TheTransform>();
        if (otherTrans != null)
        {
            TheConsole.Log("otherTrans NOT NULL");
            TheVector3 colDir = otherTrans.GlobalPosition - parent_transform.GlobalPosition; // ERROR AT: 'other.GetComponent<TheTransform>().GlobalPosition'
            //TheVector3 newRot = TheVector3.Reflect(-colDir, parent_transform.ForwardDirection);
            //float aux = newRot.z;
            //newRot.z = newRot.x;
            //newRot.x = aux;
            //parent_transform.GlobalRotation += newRot.Normalized * Mnv * TheTime.DeltaTime;
        }
		
	}

}