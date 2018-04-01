using TheEngine;
using TheEngine.TheMath;
using TheEngine.TheConsole;

public class AI_Movement {

	public TheGameObject target;
	public float yaw_rotation = 10.0f;
	public float pitch_rotation = 10.0f;
	public float roll_rotation = 10.0f;
	
	public float speed = 10.0f;

	public float avoid_distance = 50.0f;
	
	bool avoid = false;
	
	TheTransform own_trans;
	
	void Start () {
		own_trans = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		
		if(target != null)
		{
			Movement();
			
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
			TheVector3 new_rot = own_trans.LocalRotation;
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
			
			if(curr_dir.x < 0)
				curr_angle+=180;
			if(dir_to_target.x < 0)
				targ_angle+=180;
			
			if(curr_angle>targ_angle)
				new_rot.y-=TheTime.DeltaTime * yaw_rotation;
			else if(curr_angle<targ_angle)
				new_rot.y+=TheTime.DeltaTime * yaw_rotation;
			
			///Check the x rotation
			curr_angle = TheMath.Atan(curr_dir.y/curr_dir.z) * TheMath.RadToDeg;
		    targ_angle = TheMath.Atan(dir_to_target.y/dir_to_target.z) * TheMath.RadToDeg;
			
			if(curr_dir.y < 0)
				curr_angle+=180;
			if(dir_to_target.y < 0)
				targ_angle+=180;
			
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
			
			own_trans.LocalRotation = new_rot;
		}
		
		TheVector3 new_pos = own_trans.LocalPosition + own_trans.ForwardDirection * speed * TheTime.DeltaTime;
		own_trans.LocalPosition = new_pos;
	}
	
}