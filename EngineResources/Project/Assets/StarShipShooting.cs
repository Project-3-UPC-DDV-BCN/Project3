using TheEngine;
using TheEngine.TheConsole;

public class StarShipShooting {

	TheFactory laser_factory;
    public TheGameObject laser_spawner;
	
	float spawn_time = 1;
	float timer = 1;

	void Start () {
	    laser_factory = TheGameObject.Self.GetComponent<TheFactory>();
        laser_factory.StartFactory();
	}
	
	void Update () {
		if(timer <= 0)
		{
			if(TheInput.GetControllerButton(0,"LEFT_TRIGGER") == 2)
			{
				TheVector3 rot = new TheVector3(0,90,0);
				laser_factory.SetSpawnPosition(laser_spawner.GetComponent<TheTransform>().GlobalPosition);
				laser_factory.SetSpawnRotation(laser_spawner.GetComponent<TheTransform>().GlobalRotation + rot);
				laser_factory.Spawn();
				timer = spawn_time;
				TheConsole.Log("Hello");
			}
		}
		else
		{
			timer -= Time.DeltaTime;
		}
	}
}