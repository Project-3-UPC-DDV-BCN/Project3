using TheEngine;
using TheEngine.TheConsole;
using TheEngine.TheMath;

public class laser_test_script {

	public TheGameObject p;
    public TheGameObject camera;

	TheTransform camera_t;

	void Start () {
		camera_t = camera.GetComponent<TheTransform>();
	}
	
	void Update () {
		if (TheInput.IsMouseButtonDown(3))
        {
            TheGameObject go = TheGameObject.Duplicate(p);
			go.SetActive(true);
			TheVector3 pos = camera_t.GlobalPosition;
            go.GetComponent<TheRigidBody>().SetPosition(pos.x, pos.y, pos.z);
			TheVector3 rot = (camera_t.GlobalRotation.ToQuaternion() * go.GetComponent<TheTransform>().GlobalRotation.ToQuaternion()).ToEulerAngles();
			TheConsole.Log("cam rot:" + camera_t.GlobalRotation.ToQuaternion());
			TheConsole.Log("go rot:" + go.GetComponent<TheTransform>().GlobalRotation.ToQuaternion());
			TheConsole.Log("multiply rot:" + (camera_t.GlobalRotation.ToQuaternion() * go.GetComponent<TheTransform>().GlobalRotation.ToQuaternion()).ToString());
			TheConsole.Log("rot vec" + rot);
            go.GetComponent<TheRigidBody>().SetRotation(rot.x, rot.y, rot.z);
            //TheConsole.Log("go rot: " + go.GetComponent<TheTransform>().GlobalRotation);
			TheVector3 vec = camera_t.ForwardDirection * 2000 * TheTime.DeltaTime;
            go.GetComponent<TheRigidBody>().SetLinearVelocity(vec.x, vec.y, vec.z);
            //TheConsole.Log("RB rot: " + go.GetComponent<TheRigidBody>().GetRotation());
        }
        if (TheInput.IsKeyRepeat("A"))
        {
			//TheVector3 v = camera_t.GlobalRotation + new TheVector3(0, -1f, 0);
            camera_t.RotateAroundAxis(TheVector3.Up, 1);
        }
        if (TheInput.IsKeyRepeat("D"))
        {
			//TheVector3 v = camera_t.GlobalRotation + new TheVector3(0, 1f, 0);
            camera_t.RotateAroundAxis(TheVector3.Down, 1);
		}
        if (TheInput.IsKeyRepeat("W"))
        {
            //TheVector3 v = camera_t.GlobalRotation + new TheVector3(1, 0, 0);
            camera_t.RotateAroundAxis(TheVector3.Left, 1);
        }
        if (TheInput.IsKeyRepeat("S"))
        {
            //TheVector3 v = camera_t.GlobalRotation + new TheVector3(-1, 0, 0);
            camera_t.RotateAroundAxis(TheVector3.Right, 1);
        }
	}
}