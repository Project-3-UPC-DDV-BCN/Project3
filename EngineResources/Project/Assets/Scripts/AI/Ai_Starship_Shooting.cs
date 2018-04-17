using TheEngine;

public class Ai_Starship_Shooting 
{
	TheAudioSource audio_source = null;

	public TheGameObject laser_spawner_L = null;
	public TheGameObject laser_spawner_R = null;

	bool laser_spawned_left = false;

	public float laser_speed = 50000.0f;
	public float time_between_lasers = 0.100f;
	TheVector3 spawn_pos = new TheVector3(0, 0, 0);
	TheVector3 spawn_dir = new TheVector3(0, 0, 0);
	float timer = 0.0f;

	public bool shooting = false;

	public TheGameObject LaserFactoryGO = null;
	TheFactory laser_factory = null;

	TheTransform transform = null;

	public float near_plane_offset = 50f;
	public float far_plane_offset = 500f;

	public float shooting_range = 500f;
    public float shooting_angle = 60f;

	TheGameObject player = null;

	TheScript movement = null;
	bool in_range = false;

	void Start () 
	{
		movement = TheGameObject.Self.GetComponent<TheScript>(0);
		transform = TheGameObject.Self.GetComponent<TheTransform>();
		audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();

		if(laser_spawner_L == null)
			laser_spawner_L = laser_spawner_R;
		if(laser_spawner_R == null)
			laser_spawner_R = laser_spawner_L;
		if(LaserFactoryGO != null)
			laser_factory = LaserFactoryGO.GetComponent<TheFactory>();
		if(laser_factory != null) laser_factory.StartFactory();
	}
	
	void Update () 
	{
		if(LaserFactoryGO == null || laser_factory == null) 
			return;

		if(laser_spawner_L == null || laser_spawner_R == null) 
			return;

		if(player == null)
		{
			TheGameObject[] scene_obj = TheGameObject.GetSceneGameObjects();
			for(int i = 0; i < scene_obj.Length;i++)
			{
				if(scene_obj[i].tag == "Player") 
				{
					player = scene_obj[i];
					break;
				}
			}
		}

		if(player != null)
		{
			TheTransform player_trans = player.GetComponent<TheTransform>();
			if(player_trans != null)
			{
				float mag_distance = TheVector3.Distance(player_trans.GlobalPosition, transform.GlobalPosition);

				if(mag_distance < 50000)
				{
					in_range = true;
				}
				else
				{
					in_range = false;
				}
			}
		}
		//TheGameObject[] gosInFrustrum = TheGameObject.GetObjectsInFrustum(transform.GlobalPosition, transform.ForwardDirection, transform.UpDirection, near_plane_offset, far_plane_offset);
		//if(gosInFrustrum.Length > 0) shooting = true; else shooting = false;

		if(movement != null) 
		{
			TheVector3 auxTPos = TheVector3.Zero;
			auxTPos.x = movement.GetFloatField("target_pos_x");
			auxTPos.y = movement.GetFloatField("target_pos_y");
			auxTPos.z = movement.GetFloatField("target_pos_z");
			TheVector3 tOffset = auxTPos - transform.GlobalPosition;
			if(TheVector3.Magnitude(tOffset) < shooting_range && TheVector3.AngleBetween(transform.ForwardDirection, tOffset) < shooting_angle / 2)	
				shooting = true;	
			else
			{
				shooting = false;
			}
		}

		if(shooting) 
		{
			timer += TheTime.DeltaTime;
			if(timer >= time_between_lasers) 
			{
				if(laser_spawned_left) 
				{
					spawn_pos = laser_spawner_R.GetComponent<TheTransform>().GlobalPosition;
					spawn_dir = laser_spawner_R.GetComponent<TheTransform>().ForwardDirection;
					laser_spawned_left = false;
				}
				else 
				{
					spawn_pos = laser_spawner_L.GetComponent<TheTransform>().GlobalPosition;
					spawn_dir = laser_spawner_L.GetComponent<TheTransform>().ForwardDirection;
					laser_spawned_left = true;
				}
				
				laser_factory.SetSpawnPosition(spawn_pos);
				laser_factory.SetSpawnRotation(spawn_dir);
				TheGameObject laser = laser_factory.Spawn();
				TheVector3 laser_velocity = spawn_dir.Normalized * laser_speed * TheTime.DeltaTime;
				laser.GetComponent<TheTransform>().GlobalRotation = TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection;
				TheRigidBody laserBody = laser.GetComponent<TheRigidBody>();
				if(laserBody != null) 
				{
					laserBody.SetLinearVelocity(laser_velocity.x, laser_velocity.y, laser_velocity.z);			
				}	

				timer = 0.0f;
			}
			if(in_range) 
			{
				audio_source.Stop("Play_Shoot");
				audio_source.Play("Play_Shoot");
			}
		}
	}
}