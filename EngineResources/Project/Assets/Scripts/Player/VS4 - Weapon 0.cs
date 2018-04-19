using TheEngine;

using TheEngine.TheConsole;
using TheEngine.TheMath;


public class Weapon0 {	

	void Shoot(TheProgressBar weapons_bar, float curr_overheat_inc, float overheat_increment, bool used_left_laser, 
				TheFactory laser_factory, TheGameObject laser_spawner, )
	{
		TheVector3 offset;
			
		curr_overheat_inc = overheat_increment * 1.5f - overheat_increment * (weapons_bar.PercentageProgress / 100.0f);

		if (used_left_laser)
		{
			offset = new TheVector3(1, 2, 0);
		}
		else
		{
			offset = new TheVector3(-1, 2, 0);
		}
		
		used_left_laser = !used_left_laser;
		
		laser_factory.SetSpawnPosition(laser_spawner.GetComponent<TheTransform>().GlobalPosition);// + offset

		//Calculate the rotation
		TheVector3 ship_rot = laser_spawner.GetComponent<TheTransform>().GlobalRotation;

		//TheConsole.Log("ship_rot" + ship_rot);
		
		TheGameObject go = laser_factory.Spawn();

		//Set laser team parent 
		TheScript laser_scpt = go.GetComponent<TheScript>(0); 
		laser_scpt.SetStringField("parent_team", TheGameObject.Self.tag);
		laser_scpt.SetStringField("parent_team", TheGameObject.Self.GetParent().tag);

		TheVector3 vec = laser_spawner.GetComponent<TheTransform>().ForwardDirection * 20000 * TheTime.DeltaTime;
	
		if(go != null)
		{
			go.GetComponent<TheRigidBody>().SetLinearVelocity(vec.x, vec.y, vec.z);
			TheVector3 laser_rot = (ship_rot.ToQuaternion() * go.GetComponent<TheTransform>().GlobalRotation.ToQuaternion()).ToEulerAngles();
			go.GetComponent<TheRigidBody>().SetRotation(laser_rot.x, laser_rot.y, laser_rot.z);
		}
	}

    
}