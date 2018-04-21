using TheEngine;

using TheEngine.TheConsole;
using TheEngine.TheMath;


public class Weapon1 {

	private TheScript starship_shooting = null;

	void Start()
	{
		starship_shooting = TheGameObject.Self.GetComponent<TheScript>(0);
	}

	public void Shoot(TheProgressBar weapons_bar, float curr_overheat_inc, float overheat_increment, TheFactory laser_factory, 
					TheGameObject laser_spawner, TheAudioSource audio_source)
	{
		if (weapons_bar == null)
			TheConsole.Log("weapons_bar == null");
		if (curr_overheat_inc == null)
			TheConsole.Log("curr_overheat_inc == null");
		if (overheat_increment == null)
			TheConsole.Log("overheat_increment == null");
		if (laser_factory == null)
			TheConsole.Log("laser_factory == null");
		if (laser_spawner == null)
			TheConsole.Log("laser_spawner == null");
		if (audio_source == null)
			TheConsole.Log("audio_source == null");
		
		TheVector3 offset = new TheVector3(0, 2, 0);

		starship_shooting.SetFloatField("curr_overheat_inc", overheat_increment * 1.5f - overheat_increment * (weapons_bar.PercentageProgress / 100.0f));

		laser_factory.SetSpawnPosition(laser_spawner.GetComponent<TheTransform>().GlobalPosition);// + offset

		//Calculate the rotation
		TheVector3 ship_rot = laser_spawner.GetComponent<TheTransform>().GlobalRotation;

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
		
		if(audio_source != null)
		{	
			audio_source.Stop("Play_shot_2");
			audio_source.Play("Play_shot_2");
		}
	}
	
	void Cooling(float overheat_timer, float overheat, float w2_cooling_rate)
	{
		if(overheat_timer <= 0.0f)
		{
			overheat -= w2_cooling_rate * TheTime.DeltaTime;
			
			if(overheat<=0.0f)
			{
				overheat = 0.0f;
				starship_shooting.SetBoolField("overheated", false);
				starship_shooting.SetBoolField("cooling", false);
			}
			
			starship_shooting.SetFloatField("overheat", overheat);
		}	
	}
}