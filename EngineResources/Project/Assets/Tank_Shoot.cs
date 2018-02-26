using TheEngine;
using TheEngine.TheConsole;

public class Tank_Shoot {

    TheFactory missile_factory;
    public TheGameObject missile_spawner;
	
	float spawn_time = 1;
	float timer = 1;

	void Start () {
	    missile_factory = TheGameObject.Self.GetComponent<TheFactory>();
        missile_factory.StartFactory();
	}
	
	void Update () {
		if(timer <= 0)
		{
			if(TheInput.IsMouseButtonUp(1))
			{
				missile_factory.SetSpawnPosition(missile_spawner.GetComponent<TheTransform>().GlobalPosition);
				missile_factory.SetSpawnRotation(missile_spawner.GetComponent<TheTransform>().GlobalRotation);
				missile_factory.Spawn();
				timer = spawn_time;
			}
		}
		else
		{
			timer -= Time.DeltaTime;
		}
	}
}