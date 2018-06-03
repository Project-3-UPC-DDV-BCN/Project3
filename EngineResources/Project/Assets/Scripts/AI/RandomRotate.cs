using TheEngine;
using TheEngine.TheConsole;

public class RandomRotate 
{
	public float speed = 2;
	TheVector3 rotation = new TheVector3(0, 0, 0);
	TheTransform self_trans = null;	

	void Start () 
	{
		self_trans = TheGameObject.Self.GetComponent<TheTransform>();

		rotation.x = TheRandom.RandomRange(0, 1);
		rotation.y = TheRandom.RandomRange(0, 1);
		rotation.z = TheRandom.RandomRange(0, 1);

		rotation = TheVector3.Normalize(rotation);
	}
	
	void Update () 
	{
		if(self_trans != null)
		{
			TheConsole.Log(rotation.x + " " + rotation.y + " " + rotation.z);
			self_trans.LocalRotation += rotation * TheTime.DeltaTime * speed;
		}
	}
}