using TheEngine;
using TheEngine.TheConsole;
using TheEngine.TheMath;

public class adria_testing {
	
	public TheGameObject objective;
	TheTransform trans;
	TheTransform obj;
	TheQuaternion rot;
	
	void Start () {
		trans = TheGameObject.Self.GetComponent<TheTransform>();
		obj = objective.GetComponent<TheTransform>();
		rot = new TheQuaternion();
	}
	
	void Update () {
		rot = TheVector3.RotationToTarget(trans.GlobalPosition, obj.GlobalPosition).Conjugate();
		trans.GlobalPosition = TheVector3.Lerp(trans.GlobalPosition, obj.GlobalPosition, 1 * TheTime.DeltaTime);
		TheQuaternion q = new TheQuaternion();
		TheConsole.Log(rot);
		TheConsole.Log(TheQuaternion.FromEulerAngles(trans.GlobalRotation * TheMath.DegToRad));
		q = rot * TheQuaternion.FromEulerAngles(trans.GlobalRotation * TheMath.DegToRad);
		TheConsole.Log(q);
		//TheConsole.Log(q.ToEulerAngles() * TheMath.RadToDeg);
		trans.GlobalRotation = q.ToEulerAngles() * TheMath.RadToDeg;
	}
}