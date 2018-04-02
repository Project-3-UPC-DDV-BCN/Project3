using TheEngine;

using TheEngine.TheConsole;
using TheEngine.TheMath;


public class StarShipShooting {

	TheFactory laser_factory;

    public TheGameObject laser_spawner;

	public TheGameObject audio_emiter;
	//Heat BAR
	public TheGameObject overheat_bar_obj;
	TheProgressBar overheat_bar_bar = null;
	//-----------------------------------
	public TheGameObject crosshair_1;
	public TheGameObject crosshair_2;
	
	public TheGameObject weapon_icon_1;
	public TheGameObject weapon_icon_2;

	TheAudioSource audio_source;	

	public float spawn_time = 0.01f;

	float timer = 0.1f;

	bool used_left_laser = false;

    public TheGameObject weapons_energy;

    TheProgressBar weapons_bar = null;

    public float overheat_increment = 0.2f;
    float curr_overheat_inc;
    public float overheat_time = 2.0f;
    public float overheated_time = 3.0f;
    float overheat_timer = 0.0f;
    float overheat = 0.0f; //from 0.0 to 1.0
    bool overheated = false;
    public float w1_cooling_rate = 10.0f;
    public float w2_cooling_rate = 10.0f;

    public int num_weapons = 2;
    public int weapon = 0; //SELECTED WEAPON
    bool cooling = false;

    void Start () {
		laser_factory = TheGameObject.Self.GetComponent<TheFactory>();

        laser_factory.StartFactory();
		TheConsole.Log("Before Audio source");
		audio_source = audio_emiter.GetComponent<TheAudioSource>();
		if(audio_source == null) TheConsole.Log("no audio");
        weapons_bar = weapons_energy.GetComponent<TheProgressBar>();
		if(weapons_bar == null) TheConsole.Log("no weapon bar");
        curr_overheat_inc = overheat_increment;
		overheat_bar_bar = overheat_bar_obj.GetComponent<TheProgressBar>();
		if(overheat_bar_bar == null) TheConsole.Log("no overheat");
		crosshair_2.SetActive(false);
		weapon_icon_2.SetActive(false);
    }	

	void Update () {

		//Update bar
		overheat_bar_bar.PercentageProgress = overheat * 100;
		//
		if(timer <= 0 && !overheated)
		{
			if(TheInput.GetControllerJoystickMove(0,"LEFT_TRIGGER") >= 20000)
			{

                TheVector3 offset;
                switch (weapon)
                {
                    case 0:
                        {
                            curr_overheat_inc = overheat_increment * 1.5f - overheat_increment * (weapons_bar.PercentageProgress / 100.0f);

                            if (used_left_laser)
                            {
                                offset = new TheVector3(1, 2, 0);

                                used_left_laser = false;
                            }

                            else
                            {
                                offset = new TheVector3(-1, 2, 0);

                                used_left_laser = true;
                            }

                            laser_factory.SetSpawnPosition(laser_spawner.GetComponent<TheTransform>().GlobalPosition/* + offset*/);

                            //Calculate the rotation
                           // TheVector3 Z = new TheVector3(0, 0, 1);
                            TheVector3 ship_rot = laser_spawner.GetComponent<TheTransform>().GlobalRotation;

                            /*float prod = Z.x * ship_rot.x + Z.y * ship_rot.y + Z.z * ship_rot.z;
                            float magnitude_prod = Z.Length * ship_rot.Length;

                            float angle = TheMath.Acos(prod / magnitude_prod);

                            /// get the axis of this rotation
                            TheVector3 axis = TheVector3.CrossProduct(Z, ship_rot);*/
							
							TheConsole.Log("ship_rot" + ship_rot);

                            //TheVector3 laser_rot = ().ToEulerAngles();// ship_rot + new TheVector3(0, 90, 0);

                            //laser_factory.SetSpawnRotation(laser_rot);

                            TheGameObject go = laser_factory.Spawn();


							//laser_factory.SetSpawnPosition(laser_spawner.GetComponent<TheTransform>().GlobalPosition + offset);
				
                            TheVector3 vec = laser_spawner.GetComponent<TheTransform>().ForwardDirection * 20000 * TheTime.DeltaTime;

							if(go != null)
							{
								go.GetComponent<TheRigidBody>().SetLinearVelocity(vec.x, vec.y, vec.z);
								TheVector3 laser_rot = (ship_rot.ToQuaternion() * go.GetComponent<TheTransform>().GlobalRotation.ToQuaternion()).ToEulerAngles();
								go.GetComponent<TheRigidBody>().SetRotation(laser_rot.x, laser_rot.y, laser_rot.z);
							}
                       

                            timer = spawn_time;

                            audio_source.Play("Play_shot");

                            //Add heat
                            overheat += curr_overheat_inc;
                            if (overheat >= 1.0f)
                            {
                                overheated = true;
                                overheat_timer = overheated_time;
                            }
                            else overheat_timer = overheat_time;
                            break;
                        }
                    case 1:
                        {
                            if (!cooling)
                            {
                                overheat += curr_overheat_inc;
                                overheat_timer = 1.0f;
                            }
                            break;
                        }
                }
            }

            if (TheInput.GetControllerJoystickMove(0, "LEFT_TRIGGER") < 20000 && weapon == 1)
            {
                if(overheat>0.0f && !cooling)
                {
                    TheVector3 offset = new TheVector3(0, 2, 0);

                    laser_factory.SetSpawnPosition(laser_spawner.GetComponent<TheTransform>().GlobalPosition + offset);

                    TheVector3 vec = laser_spawner.GetComponent<TheTransform>().ForwardDirection * 20000 * TheTime.DeltaTime;

                    TheVector3 size = new TheVector3(1 + overheat, 1 + overheat, 1 + overheat);

                    laser_factory.SetSpawnScale(size);

                    TheGameObject go = laser_factory.Spawn();

                    go.GetComponent<TheRigidBody>().SetLinearVelocity(vec.x, vec.y, vec.z);
                    overheat_timer = 0.0f;
                    cooling = true;
                }
            }


        }

		else if(!overheated)
		{
			timer -= TheTime.DeltaTime;
		}

        if(overheat_timer<=0.0f)
        {
            //start cooling
            switch (weapon)
            {
                case 0:
                    overheat -= w1_cooling_rate * TheTime.DeltaTime; ;
                    break;
                case 1:
                    overheat -= w2_cooling_rate * TheTime.DeltaTime; ;
                    break;
            }
            
            if(overheat<=0.0f)
            {
                overheat = 0.0f;
                overheated = false;
                cooling = false;
            }
        }

        overheat_timer -= TheTime.DeltaTime;

        if(TheInput.IsKeyDown("C"))
        {
            weapon++;
            weapon %= num_weapons;

            TheVector3 size = new TheVector3(1, 1, 1);

            laser_factory.SetSpawnScale(size);
			if(crosshair_1.IsActive())
			{
				crosshair_1.SetActive(false);
				crosshair_2.SetActive(true);

				weapon_icon_1.SetActive(false);
				weapon_icon_2.SetActive(true);
			}
			else
			{
				crosshair_1.SetActive(true);
				crosshair_2.SetActive(false);

				weapon_icon_1.SetActive(true);
				weapon_icon_2.SetActive(false);
			}
        }
    }

    
}