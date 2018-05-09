using TheEngine;
using System.Collections.Generic;
using TheEngine.TheMath;
using TheEngine.TheConsole; 

public class GuillemMovement
{
	int movement_mode = 0;
	// Modes
	// 0 - Follows target
	// 1 - Follows path
	// 2 - Patrol

    public TheGameObject force_target;
    private bool forced = false;

    TheGameObject center_object;
    public float max_distance_to_center_object = 2000f;

    public float move_speed = 300;
    public float rotation_speed = 60;
	public float avoidance_rad = 30;

    private float modified_move_speed = 0;
    private float modified_rotation_speed = 0;

    TheTransform self_transform = null;

	TheGameObject target_go = null;
    TheTransform target_transform = null;

    TheTransform center_transform = null;

    TheTimer timer = new TheTimer();
    float random_time = 0;

	public float target_sphere_radius = 0f;

	float missingTimer = 0f;
	public float missSwitchTime = 3f;
	bool missSwitching = false;

	//Audio
	TheAudioSource audio_source = null;

	// Scripts ---
	TheScript entity_properties = null;
	TheScript game_manager_script = null;
	TheScript shooting_script = null;

	bool avoiding = false;
	TheVector3 avoiding_addition = new TheVector3(0, 0, 0);
	
	List<TheTransform> path = new List<TheTransform>();
	int curr_path_index = 0;
	bool path_loop = false;

	TheGameObject self = null;

	void Init()
	{
		self = TheGameObject.Self;

        entity_properties = self.GetScript("EntityProperties");
		
		TheGameObject game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");
	
		shooting_script = self.GetScript("Ai_Starship_Shooting");

	    self_transform = self.GetComponent<TheTransform>();
		audio_source = self.GetComponent<TheAudioSource>();
	}

    void Start()
    {
        RandomizeStats();

        if (center_object != null)
            center_transform = center_object.GetComponent<TheTransform>();
        else
        {
            TheGameObject[] anchors = TheGameObject.GetGameObjectsWithTag("AI_ANCHOR");
          	if(anchors.Length > 0) center_object = anchors[0];
 			 center_transform = center_object.GetComponent<TheTransform>();
        }

        if (force_target != null)
        {
            target_transform = force_target.GetComponent<TheTransform>();
            forced = true;
        }

        timer.Start();
        random_time = TheRandom.RandomRange(20, 30);
		LookForTarget();
		
		audio_source.Play("Play_Enemy_Engine");
		audio_source.SetMyRTPCvalue("Speed", modified_move_speed);
    }

    void Update()
    {
		if(audio_source != null)
			audio_source.SetMyRTPCvalue("Speed", modified_move_speed);;

		missingTimer += TheTime.DeltaTime;
		if(missingTimer > missSwitchTime) {
			missSwitching = !missSwitching;
			missingTimer = 0;
		}

		switch(movement_mode)
		{	
			case 0:
			{
				UpdateAutomaticTargetMode();
			}
			break;
			case 1:
			{
				UpdateFollowPathMode();
			}
			break;
			case 2:
			{
			}
			break;
		}

        // Move
        if (target_transform != null)
        {
            MoveFront();
            OrientateToTarget();
        }
    }

	void SetMovementMode(int set)
	{
		movement_mode = set;

		if(set != movement_mode)
		{
			if(set == 0)
				LookForTarget();
		}
	}

	void SetPathLoop(bool set)
	{
		path_loop = set;
	}

    TheVector3 GetPointInsideSphere(TheVector3 origin, float radius)
    {
        /* Thats how a random point inside a sphere is picked mathematically
		x = r * cos(theta) * cos(phi)
		y = r * sin(phi)
		z = r * sin(theta) * cos(phi)*/

        TheVector3 pos = new TheVector3(origin.x, origin.y, origin.z);

        float theta = TheRandom.RandomRange(0.0f, TheMath.PI * 2);
        float phi = TheRandom.RandomRange(0.0f, TheMath.PI * 2);

        pos.x += radius * TheMath.Cos(theta) * TheMath.Cos(phi);
        pos.y += radius * TheMath.Sin(phi);
        pos.z += radius * TheMath.Sin(theta) * TheMath.Cos(phi);

        return pos;
    }

    void UpdateAutomaticTargetMode()
	{
		// Change target after x seconds
        if (timer.ReadTime() > random_time && !forced)
        {
            target_transform = null;
            timer.Start();
            RandomizeStats();
        }

		 // Avoid leaving x point on the map 
        if (center_transform != null && self_transform != null)
        {
			TheVector3 center_trans_pos = center_transform.LocalPosition;
			TheVector3 self_trans_pos = self_transform.LocalPosition;
			
			float distance_center_self = TheVector3.Distance(center_trans_pos, self_trans_pos);               	

            if (distance_center_self > max_distance_to_center_object)
            {
                target_transform = center_transform;
            }
            else if (distance_center_self < max_distance_to_center_object / 2 &&
                    target_transform == center_transform && !forced)
            {
                target_transform = null;
            }
        }

	    // Look for a new target
        if (target_transform == null)
            LookForTarget();

		if(target_transform != null)
		{
			//TheConsole.Log(target_transform.LocalPosition.x + " " + target_transform.LocalPosition.y + " " + target_transform.LocalPosition.z);
		}
	}

	void UpdateFollowPathMode()
	{
		if(path.Count > 0 && curr_path_index >= 0 && path.Count > curr_path_index)
		{
			target_transform = path[curr_path_index];

			if(TheVector3.Distance(target_transform.GlobalPosition, self_transform.GlobalPosition) < 4)
			{
				if(path.Count > curr_path_index + 1)
					++curr_path_index;
				else if(path_loop)
					curr_path_index = 0;
				else
					target_transform = null;	
			}
		}
	}

	void SetPathParent(TheGameObject parent)
	{
		if(parent != null)
		{
			path.Clear();
			curr_path_index = 0;

			TheGameObject[] nodes = parent.GetAllChilds();

			foreach (TheGameObject node in nodes)
			{
				TheTransform trans = node.GetComponent<TheTransform>();

				if(trans != null)
				{
					path.Add(trans);
				}
			}
		}
	}

	void ClearTarget()
	{		
		target_go = null;
		target_transform = null;

		if(shooting_script != null)
			shooting_script.CallFunctionArgs("ClearTargetTransform");
	}

	void ClearIfTarget(TheGameObject go)
	{
		if(go.GetComponent<TheTransform>() == target_transform)
		{
			//TheConsole.Log("My target was destroyed! Reseting..");
			ClearTarget();
		}
	}

	TheTransform GetTargetTransform()
	{
		return target_transform;
	}

    void LookForTarget()
    {
		if(entity_properties == null) 
			return;

		string factionStr = (string)entity_properties.CallFunctionArgs("GetFaction");

		List<TheGameObject> enemy_ships = new List<TheGameObject>();	

		if(factionStr == "alliance") 
			enemy_ships = (List<TheGameObject>)game_manager_script.CallFunctionArgs("GetEmpireShips");
		
		else if(factionStr == "empire") 
			enemy_ships = (List<TheGameObject>)game_manager_script.CallFunctionArgs("GetAllianceShips");
			
		TheGameObject PlayerGo = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");
		
		if(PlayerGo != null) {
			// Player faction Filter Here ...
			// ...
			enemy_ships.Add(PlayerGo);
		}

		if(enemy_ships.Count > 0) 
		{
			int random = (int)TheRandom.RandomRange(0, enemy_ships.Count);

			target_go = enemy_ships[random];
			target_transform = target_go.GetComponent<TheTransform>();
			
			object[] args = {target_transform};
			if(shooting_script != null)
				shooting_script.CallFunctionArgs("SetTargetTransform", args);
		}
    }

    void MoveFront()
    {
        if (target_transform != null)
        {
            TheVector3 pos = self_transform.LocalPosition;
			TheVector3 forward_dir = self_transform.ForwardDirection;

            TheVector3 move = new TheVector3(0, 0, 0);
            move.x = modified_move_speed * forward_dir.x;
            move.y = modified_move_speed * forward_dir.y;
            move.z = modified_move_speed * forward_dir.z;

            self_transform.LocalPosition = new TheVector3(pos.x + (move.x * TheTime.DeltaTime), pos.y + (move.y * TheTime.DeltaTime), pos.z + (move.z * TheTime.DeltaTime));
        }
    }

    void OrientateToTarget()
    {
		TheVector3 self_pos = self_transform.GlobalPosition;
		TheVector3 target_pos = new TheVector3();
		if(missSwitching == true) {
			 target_pos = GetPointInsideSphere(target_transform.GlobalPosition, target_sphere_radius);
		}
		else {
			target_pos = target_transform.GlobalPosition;
		}
		
		TheVector3 self_trans_rot = self_transform.LocalRotation;
		
		// Security (sometimes they go wild)
		if(self_trans_rot.x > 180 || self_trans_rot.x < -180) 
		{
			TheConsole.Log("IM WRONG!!!");
			self_transform.LocalRotation = new TheVector3(0, self_trans_rot.y, self_trans_rot.z);
			self_trans_rot = self_transform.LocalRotation;
		}

		// Target avoidance ----
		if(movement_mode == 0)
		{
			bool was_avoiding = avoiding;
			if(TheVector3.Distance(self_pos, target_pos) < avoidance_rad)
				avoiding = true;
			else
				avoiding = false;
		
			if(!was_avoiding && avoiding)
			{
				avoiding_addition = GetRandomAvoidance();
			}
 
			if(TheVector3.Distance(self_pos, target_pos) < avoidance_rad)
			{
				target_pos.x += avoiding_addition.x;
				target_pos.y += avoiding_addition.y;
				target_pos.z += avoiding_addition.z;
			}
		}
		// --------------------

        float target_x_angle = -GetAngleFromTwoPoints(self_pos.x, self_pos.z, target_pos.x, target_pos.z) - 270;

        float angle_diff_x = self_trans_rot.y - target_x_angle;

		////TheConsole.Log("x: " + NormalizeAngle(angle_diff_x));
	
		if((NormalizeAngle(self_trans_rot.x) < 90 && NormalizeAngle(self_trans_rot.x) > -90) || 
		(NormalizeAngle(self_trans_rot.x) > 270 || NormalizeAngle(self_trans_rot.x) < -270))
		{
        	if (NormalizeAngle(angle_diff_x) > 180)
			{
				////TheConsole.Log("1");
            	self_transform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y - (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.z);
			}
        	else
			{
           	 	self_transform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y + (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.z);
				////TheConsole.Log("2");
			}
		}
		else
		{
			if (NormalizeAngle(angle_diff_x) > 0)
			{
				////TheConsole.Log("1-1");
            	self_transform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y - (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.z);
			}
        	else
			{
           	 	self_transform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y + (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.z);
				////TheConsole.Log("2-2");
			}
		}
		
		
		// Y
	
		self_trans_rot = self_transform.LocalRotation;
			
		float target_y_angle = GetAngleFromTwoPoints(self_pos.y, self_pos.z, target_pos.y, target_pos.z) - 270;

		////TheConsole.Log("y: " + NormalizeAngle(target_y_angle));	

		////TheConsole.Log("y: " + NormalizeAngle(angle_diff_y));
		if(self_pos.z > target_pos.z)
		{
			target_y_angle -= 180;
		}		

        float angle_diff_y =self_trans_rot.x - target_y_angle;		

		if (NormalizeAngle(angle_diff_y) > 180)
		{
			////TheConsole.Log("1");
			self_transform.LocalRotation = new TheVector3(self_trans_rot.x + (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.y, self_trans_rot.z);
		}
		else
		{
			////TheConsole.Log("2");
			self_transform.LocalRotation = new TheVector3(self_trans_rot.x - (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.y, self_trans_rot.z);
		}
    }

    float GetAngleFromTwoPoints(float x1, float y1, float x2, float y2)
    {
        float xDiff = x1 - x2;
        float yDiff = y1 - y2;
        return (float)TheMath.Atan2(yDiff, xDiff) * (float)(180 / TheMath.PI);
    }

    float NormalizeAngle(float angle)
    {
        if (angle > 360)
        {
            int multiplers = (int)(angle / 360);
            angle -= (360 * multiplers);
        }

        if (angle < -360)
        {
            int multiplers = (int)(System.Math.Abs(angle) / 360);
            angle += (360 * multiplers);
        }

        return angle;
    }
	
	TheVector3 GetRandomAvoidance()
	{
		TheVector3 ret = new TheVector3(0, 0, 0);

		int rand = (int)TheRandom.RandomRange(0, 5);

		if(rand < 1)
		{
			ret.x = avoidance_rad;
			ret.y = 0;
			ret.z = avoidance_rad;
		}
		else if(rand < 2)
		{
			ret.x = -avoidance_rad;
			ret.y = 0;
			ret.z = -avoidance_rad;
		}
		else if(rand < 3)
		{
			ret.x = 0;
			ret.y = avoidance_rad;
			ret.z = avoidance_rad;
		}
		else
		{
			ret.x = 0;
			ret.y = -avoidance_rad;
			ret.z = -avoidance_rad;
		}

		return ret;
	}

    void RandomizeStats()
    {
        float move_min = move_speed - (float)(move_speed / 10);
        float move_max = move_speed + (float)(move_speed / 10);
        float rotation_min = rotation_speed - (float)(rotation_speed / 10);
        float rotation_max = rotation_speed + (float)(rotation_speed / 10);

        modified_move_speed = TheRandom.RandomRange(move_min, move_max);
        modified_rotation_speed = TheRandom.RandomRange(rotation_min, rotation_max);
    }
}