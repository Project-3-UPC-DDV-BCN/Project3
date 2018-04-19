using TheEngine;
using TheEngine.TheConsole;

public class Laser 
{
	public float laser_damage = 10.0f;

	private TheGameObject sender = null;
	private TheTransform self_trans = null;

	private float speed = 0.0f;
	private TheVector3 direction = new TheVector3(0, 0, 0);
	private TheQuaternion orientation = TheQuaternion.Identity;

	void Init()
	{
		self_trans = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update()
	{
		MoveFront();
	}

	// Set laser info
	void SetInfo(TheGameObject send, float sp, TheVector3 dir, TheQuaternion ori)
	{
		sender = send;
		speed = sp;
		direction = dir; 
		orientation = ori;
	}

	TheGameObject GetSender()
	{
		return sender;
	}

	// Check collision and notify collided ship
	void OnTriggerEnter(TheCollisionData coll)
	{
		if(coll == null)
			return;

		TheGameObject other_ship = coll.Collider.GetGameObject();

		if(other_ship == null)
			return;
		
		if(other_ship.tag != "ShipEntity")
			return;

		TheScript ship_properties = other_ship.GetScript("ShipProperties");
		
		if(ship_properties != null && sender != null)
		{
			// I could do this comparing only the game objects but for some reason it's not working :(
			if(other_ship.GetComponent<TheTransform>() != sender.GetComponent<TheTransform>())
			{
				TheConsole.Log(other_ship.name + " " + sender.name);
				object[] args = {sender};
				ship_properties.CallFunctionArgs("HitByShip", args);
				TheGameObject.Self.SetActive(false);
			}
		}
	}

	void MoveFront()
    {
		if(sender != null)
		{
			if(self_trans != null)
			{
				float dt = TheTime.DeltaTime;

				TheVector3 add = new TheVector3(0, 0, 0);
				add.x = direction.x * speed * dt;
				add.y = direction.y * speed * dt;
				add.z = direction.z * speed * dt;

				TheVector3 curr = self_trans.LocalPosition;
				self_trans.LocalPosition = new TheVector3(curr.x + add.x, curr.y + add.y, curr.z + add.z);
			}
		}
    }
}