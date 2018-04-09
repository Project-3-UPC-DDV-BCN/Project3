using TheEngine;
using TheEngine.TheConsole;

public class collision_test {
	
	public int hp = 50;
	TheTransform trans;
	
	public TheGameObject bot_right;
	public TheGameObject bot_left;
	public TheGameObject top_right;
	public TheGameObject top_left;

	void Start () {
		trans = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		
	}
	
	void OnCollisionEnter(TheCollisionData col_data)
	{
		ThePhysics.Explosion(col_data.ContactPoints[0].Position, 25, 20);
	}
	
	void OnTriggerEnter(TheCollisionData col_data)
	{
		hp -= 10;
		
		if(hp <= 0)
		{
			TheCollider col1 = bot_right.GetComponent<TheCollider>();
			if(col1 != null)
			{
				col1.SetComponentActive(true);
				TheRigidBody rb = col1.GetRigidBody();
				if(rb != null)
				{
					rb.TransformGO = true;
				}
			}
			TheCollider col2 = bot_left.GetComponent<TheCollider>();
			if(col2 != null)
			{
				col2.SetComponentActive(true);
				TheRigidBody rb = col2.GetRigidBody();
				if(rb != null)
				{
					rb.TransformGO = true;
				}
			}
			TheCollider col3 = top_right.GetComponent<TheCollider>();
			if(col3 != null)
			{
				col3.SetComponentActive(true);
				TheRigidBody rb = col3.GetRigidBody();
				if(rb != null)
				{
					rb.TransformGO = true;
				}
			}
			TheCollider col4 = top_left.GetComponent<TheCollider>();
			if(col4 != null)
			{
				col4.SetComponentActive(true);
				TheRigidBody rb = col4.GetRigidBody();
				if(rb != null)
				{
					rb.TransformGO = true;
				}
			}
			TheRigidBody self_rb = TheGameObject.Self.GetComponent<TheRigidBody>();
			if(self_rb != null)
			{
				self_rb.TransformGO = true;
			}
			ThePhysics.Explosion(trans.GlobalPosition - new TheVector3(0,5,10), 25, 20);
		}
	}
}