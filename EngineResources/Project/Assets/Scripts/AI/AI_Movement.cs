using TheEngine;
using TheEngine.TheMath;
using TheEngine.TheConsole;

public class AI_Movement {

	TheGameObject target = null;
	TheGameObject gameobject;
	TheTransform own_trans;
	
	public float yaw_rotation = 10.0f;
	public float pitch_rotation = 10.0f;
	public float roll_rotation = 10.0f;
	
	public float speed = 10.0f;

	public float avoid_distance = 50.0f;
	
	bool avoid = false;
	
	public TheGameObject laser_spawner_L = null;
	public TheGameObject laser_spawner_R = null;
	TheFactory laser_factory;
	bool laser_spawned_left = false;
	public float laser_speed = 20000.0f;
	public float time_between_lasers = 0.02f;
	TheVector3 spawn_pos = new TheVector3();
	TheVector3 spawn_dir = new TheVector3();
	float timer = 0.0f;

	public float sight_range = 100.0f;
	public float sight_angle = 60.0f;

	public bool shooting = false;
	
	void Start () {
		own_trans = TheGameObject.Self.GetComponent<TheTransform>();
		gameobject = TheGameObject.Self;
		laser_factory = TheGameObject.Self.GetComponent<TheFactory>();
		laser_factory.StartFactory();
		if(laser_spawner_L == null)
			laser_spawner_L = laser_spawner_R;
		if(laser_spawner_R == null)
			laser_spawner_R = laser_spawner_L;
	}
	
	void Update () {
		
		if(target != null)
		{
			Movement();
			Shoot();
		}
		else
		{
			GetNewTarget();
		}
	}
	
	void Movement(){
		
		if(!avoid)
		{
			//calculate the direction to the target
			TheTransform target_trans = target.GetComponent<TheTransform>();
			TheVector3 dir_to_target = target_trans.GlobalPosition - own_trans.GlobalPosition;
			dir_to_target = TheVector3.Normalize(dir_to_target);
				
			//compare this to our direction and rotate to target
			TheVector3 curr_dir = own_trans.ForwardDirection;
			TheVector3 new_rot = own_trans.GlobalRotation;
			/////Check the z rotation
			//float curr_angle = TheMath.Atan(curr_dir.y/curr_dir.x) * TheMath.RadToDeg;
			//float targ_angle = TheMath.Atan(dir_to_target.y/dir_to_target.x) * TheMath.RadToDeg;
			//
			//if(curr_dir.y < 0)
			//	curr_angle+=180;
			//if(target_trans.GlobalPosition.y < own_trans.GlobalPosition.y)
			//	targ_angle-=180;
			//
			//if(curr_angle>targ_angle)
			//	new_rot.z-=TheTime.DeltaTime * roll_rotation;
			//else if(curr_angle<targ_angle)
			//	new_rot.z+=TheTime.DeltaTime * roll_rotation;
			
			///Check the y rotation
			float curr_angle = TheMath.Atan(curr_dir.x/curr_dir.z) * TheMath.RadToDeg;
		    float targ_angle = TheMath.Atan(dir_to_target.x/dir_to_target.z) * TheMath.RadToDeg;
			
			if(curr_dir.z < 0)
				curr_angle+=180;
			if(dir_to_target.z < 0)
				targ_angle-=180;
			
			if(curr_angle>targ_angle)
				new_rot.y-=TheTime.DeltaTime * yaw_rotation;
			else if(curr_angle<targ_angle)
				new_rot.y+=TheTime.DeltaTime * yaw_rotation;
			
			///Check the x rotation
			curr_angle = TheMath.Atan(curr_dir.y/curr_dir.z) * TheMath.RadToDeg;
		    targ_angle = TheMath.Atan(dir_to_target.y/dir_to_target.z) * TheMath.RadToDeg;
			
			if(curr_dir.z < 0)
				curr_angle+=180;
			if(dir_to_target.z < 0)
				targ_angle-=180;
			
			TheConsole.Log("---X Rot---");
			TheConsole.Log(curr_angle);
			TheConsole.Log(targ_angle);
			
			if(curr_angle>targ_angle)
				new_rot.x-=TheTime.DeltaTime * pitch_rotation;
			else if(curr_angle<targ_angle)
				new_rot.x+=TheTime.DeltaTime * pitch_rotation;
			
			TheConsole.Log(new_rot.x);
			new_rot.x %= 360;
			new_rot.y %= 360;
			new_rot.z %= 360;
			
			own_trans.GlobalRotation = new_rot;
		}
		
		TheVector3 new_pos = own_trans.LocalPosition + own_trans.ForwardDirection * speed * TheTime.DeltaTime;
		own_trans.LocalPosition = new_pos;
	}
	
	void GetNewTarget() {
    	TheGameObject[] ships_in_scene = new TheGameObject[500];
		//List<TheGameObject> ships_in_scene = new List<TheGameObject>();
        int nship = 0;
        foreach (TheGameObject go in TheGameObject.GetSceneGameObjects())
        {
  	      if(gameobject.tag == "Alliance" && go.tag == "Empire" || gameobject.tag == "Empire" && go.tag == "Alliance")
          {
   			//ships_in_scene.Add(go);               
			ships_in_scene[nship++] = go;
          }
        }
		int newS = 0;
		foreach(TheGameObject go in ships_in_scene) {
			if(go == null)
				break;
			newS++;
		}
		TheGameObject[] auxList = new TheGameObject[newS];
		for(int i = 0; i < newS; i++) {
			auxList[i] = ships_in_scene[i];
		}
        if(auxList.Length > 0)
        {
			target = auxList[0];
			foreach(TheGameObject ship in auxList) {
				float distA = TheVector3.Magnitude(own_trans.GlobalPosition - ship.GetComponent<TheTransform>().GlobalPosition);
				float currDistB  = TheVector3.Magnitude(own_trans.GlobalPosition - target.GetComponent<TheTransform>().GlobalPosition);
				if(currDistB > distA) {
					//if(target != auxTarget && target != null)
					target = ship;
				}
			}
			int rand = (int)TheRandom.RandomRange(0f, auxList.Length);
			target = ships_in_scene[rand];
   	   }
	   else
	   {	
			target = ships_in_scene[1];
	   }
 
		//TheGameObject ret = null;
		//TheGameObject[] scene_gos = TheGameObject.GetSceneGameObjects();
		//int nS = 0;
		//foreach(TheGameObject go in scene_gos) {
		//	if(isEnemy(gameobject.tag, go.tag)) {
		//		nS++;
		//	}
		//}
		//if(nS <= 0)
		//	return null;
		//TheGameObject[] enemy_ships = new TheGameObject[nS];
		//int it = 0;
		//for(int i = 0; i < scene_gos.Length; i++) {
		//	if(isEnemy(gameobject.tag, scene_gos[i].tag))
		//		if(scene_gos[i] is TheGameObject)
		//			enemy_ships[it++] = scene_gos[i];
		//		else
		//			return null;
		//}
		//int rand_n = (int)TheRandom.RandomRange(0, nS);
		//ret = enemy_ships[rand_n];
		//return ret;
	}
	
	void Shoot()
	{
		TheTransform ttrans = target.GetComponent<TheTransform>();
		TheVector3 ship_dir = ttrans.GlobalPosition - own_trans.GlobalPosition;
		if(TheVector3.Magnitude(ship_dir) < sight_range && TheVector3.AngleBetween(own_trans.ForwardDirection, ship_dir) < sight_angle/2)
			shooting = true;
		else
			shooting = false;
			
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