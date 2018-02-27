using TheEngine;
using TheEngine.TheConsole;

public class StarShipShooting {

	TheFactory laser_factory;
    public TheGameObject laser_spawner;
	
	float spawn_time = 0.5f;
	float timer = 0.5f;
	bool used_left_laser = false;

	void Start () {
	    laser_factory = TheGameObject.Self.GetComponent<TheFactory>();
        laser_factory.StartFactory();
	}
	
	void Update () {
		if(timer <= 0)
		{
			if(TheInput.GetControllerButton(0,"LEFT_TRIGGER") == 2)
			{
				//TheVector3 rot = new TheVector3(0,0,0);
				TheVector3 offset;
				if(used_left_laser)
				{
					offset = new TheVector3(2,0,0);
					used_left_laser = false;
				}
				else
				{
					offset = new TheVector3(-2,0,0);
					used_left_laser = true;
				}
				laser_factory.SetSpawnPosition(laser_spawner.GetComponent<TheTransform>().GlobalPosition + offset);
				laser_factory.SetSpawnRotation(laser_spawner.GetComponent<TheTransform>().GlobalRotation);
				laser_factory.Spawn();
				timer = spawn_time;
			}
		}
		else
		{
			timer -= TheTime.DeltaTime;
		}
	}
}