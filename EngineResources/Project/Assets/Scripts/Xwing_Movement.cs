using TheEngine;
using TheEngine.TheConsole;

public class Xwing_Movement {

	TheTransform trans;
	public int hp = 50;
	public bool is_dead = false;
	public TheVector3 explosion_pos;

	void Start () {
		trans = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		if(TheInput.IsKeyDown("X"))
		{
			ThePhysics.Explosion(trans.GlobalPosition - explosion_pos, 100, 5);
		}
		if(!is_dead)
		{
			//trans.GlobalPosition += trans.ForwardDirection * TheTime.DeltaTime * 5;
		}
	}

	void OnTriggerEnter(TheGameObject other)
	{
		TheConsole.Log("Hitted!");
		hp -= 10;
		if(hp <= 0)
		{
			is_dead = true;
			TheRigidBody self_rb = TheGameObject.Self.GetComponent<TheRigidBody>();
			if(self_rb != null)
			{
				//TheGameObject.Self.DestroyComponent(self_rb);
			}
			TheGameObject[] childs = TheGameObject.Self.GetAllChilds();
			foreach(TheGameObject child in childs)
			{
				TheRigidBody rb = child.GetComponent<TheRigidBody>();
				if(rb != null)
				{
					TheTransform t = child.GetComponent<TheTransform>();
					rb.TransformGO = true;
					/*float x_velo = TheRandom.RandomRange(1,10);
					float y_velo = TheRandom.RandomRange(1,10);
					float z_velo = TheRandom.RandomRange(1,10);
					rb.SetLinearVelocity(x_velo, y_velo, z_velo);*/
				}
			}
		}
	}
}