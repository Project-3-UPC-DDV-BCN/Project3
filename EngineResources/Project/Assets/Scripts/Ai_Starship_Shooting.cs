using TheEngine;

public class Ai_Starship_Shooting {

	public TheGameObject laser_spawner_L = null;
	public TheGameObject laser_spawner_R = null;
	bool laser_spawned_left = false;
	public float laser_speed = 50000.0f;
	public float time_between_lasers = 0.100f;
	TheVector3 spawn_pos = new TheVector3();
	TheVector3 spawn_dir = new TheVector3();
	float timer = 0.0f;

	public bool shooting = false;

	public TheGameObject LaserFactoryGO = null;
	TheFactory laser_factory = null;

	TheTransform transform;

	public float near_plane_offset = 50f;
	public float far_plane_offset = 500f;

	public float shooting_range = 500f;

	TheScript movement = null;

	void Start () {
		movement = TheGameObject.Self.GetComponent<TheScript>(0);
		transform = TheGameObject.Self.GetComponent<TheTransform>();
		if(laser_spawner_L == null)
			laser_spawner_L = laser_spawner_R;
		if(laser_spawner_R == null)
			laser_spawner_R = laser_spawner_L;
		if(LaserFactoryGO != null)
			laser_factory = LaserFactoryGO.GetComponent<TheFactory>();
		if(laser_factory != null) laser_factory.StartFactory();
	}
	
	void Update () {
		if(LaserFactoryGO == null || laser_factory == null) return;
		if(laser_spawner_L == null || laser_spawner_R == null) return;

		//TheGameObject[] gosInFrustrum = TheGameObject.GetObjectsInFrustum(transform.GlobalPosition, transform.ForwardDirection, transform.UpDirection, near_plane_offset, far_plane_offset);
		//if(gosInFrustrum.Length > 0) shooting = true; else shooting = false;

		if(movement != null) {
			TheVector3 tOffset = movement.GetVector3Field("target_pos") - transform.GlobalPosition;
			if(TheVector3.Magnitude(tOffset) < shooting_range)
				shooting = true;
			else
				shooting = false;
		}

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
				laser_factory.SetSpawnRotation(spawn_dir);
				TheGameObject laser = laser_factory.Spawn();
				TheVector3 laser_velocity = spawn_dir.Normalized * laser_speed * TheTime.DeltaTime;
				laser.GetComponent<TheTransform>().GlobalRotation = TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection;
				TheRigidBody laserBody = laser.GetComponent<TheRigidBody>();
				if(laserBody != null) {
					laserBody.SetLinearVelocity(laser_velocity.x, laser_velocity.y, laser_velocity.z);			
				}	

				timer = 0.0f;
			}
		}
		
	}
}