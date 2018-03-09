using TheEngine;

using TheEngine.TheConsole;
using TheEngine.Math;


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
				
				//Calculate the rotation
				TheVector3 Z = new TheVector3(0,0,1);
				TheVector3 ship_rot = laser_spawner.GetComponent<TheTransform>().GlobalRotation;
				
				float prod = Z.x*ship_rot.x+Z.y*ship_rot.y+Z.z*ship_rot.z;
				float magnitude_prod = Z.Length*ship_rot.Length;
				
				float angle = TheMath.Acos(prod/magnitude_prod);

				/// get the axis of this rotation
				TheVector3 axis = TheVector3.CrossProduct(Z,ship_rot);

				TheVector3 laser_rot = new TheVector3(0,90 + ship_rot.y,0);

				laser_factory.SetSpawnRotation(laser_rot);

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