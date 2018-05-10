using TheEngine;
using TheEngine.TheConsole;

public class Arrow {
	
	private TheScript targeting_script;
	public TheGameObject arrow_go = null;
	
	private TheGameObject curr_target = null;
	private TheGameObject self = null;
	//Angle variables
	private TheVector3 original_angle = TheVector3.Up;
	private TheVector3 curr_angle = null;
	
	private TheVector3 original_arrow_pos = null;
	public float radius = 5f;
	
	
	void Start () {
		self = TheGameObject.Self;
		if(self != null)
			targeting_script = TheGameObject.Self.GetScript("Targeting");
		if(targeting_script != null)
			curr_target = (TheGameObject)targeting_script.CallFunctionArgs("GetTarget");
		if(arrow_go != null)
			original_arrow_pos = arrow_go.GetComponent<TheTransform>().GlobalPosition;
		
	}
	
	void Update () {
		if(curr_target != null)
		{
			TheVector3 angle = curr_target.GetComponent<TheTransform>().GlobalPosition - self.GetComponent<TheTransform>().GlobalPosition;
			angle.z = 0;
			curr_angle = angle.Normalized;
		}
		if(arrow_go != null)
		{
			//Update Position
			arrow_go.GetComponent<TheTransform>().GlobalPosition = original_arrow_pos + radius * curr_angle;
			//Rotate Arrow
			
			
		}
		
		
	}
}