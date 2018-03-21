using TheEngine;

public class Ai_Starship_Shooting {

	public TheGameObject laser_spawner_L = null;
	public TheGameObject laser_spawner_R = null;
	TheFactory laser_factory;
	bool laser_spawned_left = false;
	public float laser_speed = 20000.0f;
	public float time_between_lasers = 0.02f;
	TheVector3 spawn_pos = new TheVector3();
	TheVector3 spawn_dir = new TheVector3();
	float timer = 0.0f;

	public bool shooting = false;

	void Start () {
		laser_factory = TheGameObject.Self.GetComponent<TheFactory>();
		laser_factory.StartFactory();
		if(laser_spawner_L == null)
			laser_spawner_L = laser_spawner_R;
		if(laser_spawner_R == null)
			laser_spawner_R = laser_spawner_L;
	}
	
	void Update () {
		if(laser_spawner_L == null || laser_spawner_R == null)
			return;
		if(shooting) {
			timer += TheTime.DeltaTime;
			if(timer >= time_between_lasers) {
				if(laser_spawned_left) {
					spawn_pos = laser_spawner_R.GetComponent<TheTransform>().GlobalPosition;
					spawn_dir = laser_spawner_R.GetComponent<TheTransform>().ForwardDirection;
					laser_spawned_left = false;
				}
				else {
					spawn_pos = laser_spawner_L.GetComponent<TheTransform>().GlobalPosition;
					spawn_dir = laser_spawner_L.GetComponent<TheTransform>().ForwardDirection;
					laser_spawned_left = true;
				}
				laser_factory.SetSpawnPosition(spawn_pos);
				TheGameObject laser = laser_factory.Spawn();
				TheVector3 laser_velocity = spawn_dir.Normalized * laser_speed * TheTime.DeltaTime;
				laser.GetComponent<TheRigidBody>().SetLinearVelocity(laser_velocity.x, laser_velocity.y, laser_velocity.z);				

				timer = 0.0f;
			}
		}
		
	}
}