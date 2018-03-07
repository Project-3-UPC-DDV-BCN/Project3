using TheEngine;

using TheEngine.TheConsole;



public class StarShipShooting {



	TheFactory laser_factory;

    public TheGameObject laser_spawner;

	public TheGameObject audio_emiter;
	
	TheAudioSource audio_source;	

	public float spawn_time = 0.01f;

	float timer = 0.1f;

	bool used_left_laser = false;



	void Start () {

	    laser_factory = TheGameObject.Self.GetComponent<TheFactory>();

        laser_factory.StartFactory();
		
		audio_source = audio_emiter.GetComponent<TheAudioSource>();

	}

	

	void Update () {

		if(timer <= 0)

		{

			if(TheInput.GetControllerJoystickMove(0,"LEFT_TRIGGER") >= 20000)

			{

				//TheVector3 rot = new TheVector3(0,0,0);

				TheVector3 offset;

				if(used_left_laser)

				{

					offset = new TheVector3(1,2,0);

					used_left_laser = false;

				}

				else

				{

					offset = new TheVector3(-1,2,0);

					used_left_laser = true;

				}

				laser_factory.SetSpawnPosition(laser_spawner.GetComponent<TheTransform>().GlobalPosition + offset);

				//laser_factory.SetSpawnRotation(laser_spawner.GetComponent<TheTransform>().GlobalRotation);

				TheGameObject go = laser_factory.Spawn();

				TheVector3 vec = laser_spawner.GetComponent<TheTransform>().ForwardDirection*20000*TheTime.DeltaTime;

				go.GetComponent<TheRigidBody>().SetLinearVelocity(vec.x, vec.y, vec.z);

				timer = spawn_time;

				//TheConsole.Log("Spawner " + laser_spawner.GetComponent<TheTransform>().ForwardDirection);
				audio_source.Play("Play_shot1");
			}

		}

		else

		{

			timer -= TheTime.DeltaTime;

		}

	}

}