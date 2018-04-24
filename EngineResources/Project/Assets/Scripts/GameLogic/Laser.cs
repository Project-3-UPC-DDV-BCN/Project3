using TheEngine;
//using TheEngine.TheConsole; 

public class Laser 
{
	public int laser_damage = 10;

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
	void SetInfo(TheGameObject _sender, float _speed, int _damage, TheVector3 dir, TheQuaternion ori)
	{
		sender = _sender;
		speed = _speed;
		direction = dir; 
		orientation = ori;
		laser_damage = _damage;
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
		
		if(other_ship.tag != "Entity")
			return;

		TheScript ship_properties = other_ship.GetScript("EntityProperties");
		
		if(ship_properties != null && sender != null)
		{
			// I could do this comparing only the game objects but for some reason it's not working :(
			if(other_ship.GetComponent<TheTransform>() != sender.GetComponent<TheTransform>())
			{
				object[] args = {sender, laser_damage};
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
				self_trans.QuatRotation = orientation;
			}
		}
    }
}