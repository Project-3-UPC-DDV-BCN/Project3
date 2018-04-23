using TheEngine;
using System.Collections.Generic;
using TheEngine.TheMath;
using TheEngine.TheConsole;

public class GuillemMovement
{
    public TheGameObject force_target;
    private bool forced = false;

    TheGameObject center_object;
    public float max_distance_to_center_object = 2000f;
    public float desertor_distance = 5000f;

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

	//Audio
	TheAudioSource audio_source = null;

	// Scripts ---
	TheScript ShipProperties = null;
	TheScript GameManager = null;
	TheScript shooting_script = null;

	bool avoiding = false;
	TheVector3 avoiding_addition = new TheVector3(0, 0, 0);

	void Init()
	{
        ShipProperties = TheGameObject.Self.GetScript("ShipProperties");
		
		TheGameObject GM = TheGameObject.Find("GameManager");
		if(GM != null)
			GameManager = GM.GetScript("GameManager");
	
		shooting_script = TheGameObject.Self.GetScript("Ai_Starship_Shooting");

	    self_transform = TheGameObject.Self.GetComponent<TheTransform>();
		audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();
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
		audio_source.SetMyRTPCvalue("Speed",modified_move_speed);// is this the current speed of the ship? Change it if not pls
    }

    void Update()
    {
		if(audio_source != null)
			audio_source.SetMyRTPCvalue("Speed", modified_move_speed);

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
			TheVector3 center_trans_pos = center_transform.GlobalPosition;
			TheVector3 self_trans_pos = self_transform.GlobalPosition;
			
			float distance_center_self = TheVector3.Distance(center_trans_pos, self_trans_pos);
			
            if (distance_center_self > desertor_distance)
				self_transform.GlobalPosition = center_transform.GlobalPosition;               	

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

        // Move
        if (target_transform != null)
        {
            MoveFront();
            OrientateToTarget();
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
			TheConsole.Log("My target was destroyed! Reseting..");
			ClearTarget();
		}
	}

	TheTransform GetTargetTransform()
	{
		return target_transform;
	}

    void LookForTarget()
    {
		if(ShipProperties == null) 
			return;

		string factionStr = (string)ShipProperties.CallFunctionArgs("GetFaction");

		List<TheGameObject> enemy_ships = new List<TheGameObject>();	

		if(factionStr == "alliance") 
			enemy_ships = (List<TheGameObject>)GameManager.CallFunctionArgs("GetEmpireShips");
		
		else if(factionStr == "empire") 
			enemy_ships = (List<TheGameObject>)GameManager.CallFunctionArgs("GetAllianceShips");
			
		TheGameObject PlayerGo = (TheGameObject)GameManager.CallFunctionArgs("GetSlave1");
		
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
        TheVector3 target_pos = target_transform.GlobalPosition;
		
		TheVector3 self_trans_rot = self_transform.LocalRotation;
		
		// Target avoidance ----
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
		// --------------------

        float target_x_angle = -GetAngleFromTwoPoints(self_pos.x, self_pos.z, target_pos.x, target_pos.z) - 270;

        float angle_diff_x = self_trans_rot.y - target_x_angle;

		//TheConsole.Log("x: " + NormalizeAngle(angle_diff_x));
	
		if((NormalizeAngle(self_trans_rot.x) < 90 && NormalizeAngle(self_trans_rot.x) > -90) || 
		(NormalizeAngle(self_trans_rot.x) > 270 || NormalizeAngle(self_trans_rot.x) < -270))
		{
        	if (NormalizeAngle(angle_diff_x) > 180)
			{
				//TheConsole.Log("1");
            	self_transform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y - (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.z);
			}
        	else
			{
           	 	self_transform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y + (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.z);
				//TheConsole.Log("2");
			}
		}
		else
		{
			if (NormalizeAngle(angle_diff_x) > 0)
			{
				//TheConsole.Log("1-1");
            	self_transform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y - (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.z);
			}
        	else
			{
           	 	self_transform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y + (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.z);
				//TheConsole.Log("2-2");
			}
		}
		
		
		// Y
	
		self_trans_rot = self_transform.LocalRotation;
			
		float target_y_angle = GetAngleFromTwoPoints(self_pos.y, self_pos.z, target_pos.y, target_pos.z) - 270;

		//TheConsole.Log("y: " + NormalizeAngle(target_y_angle));	

		//TheConsole.Log("y: " + NormalizeAngle(angle_diff_y));
		if(self_pos.z > target_pos.z)
		{
			target_y_angle -= 180;
		}		

        float angle_diff_y =self_trans_rot.x - target_y_angle;		

		if (NormalizeAngle(angle_diff_y) > 180)
		{
			//TheConsole.Log("1");
			self_transform.LocalRotation = new TheVector3(self_trans_rot.x + (modified_rotation_speed * TheTime.DeltaTime), self_trans_rot.y, self_trans_rot.z);
		}
		else
		{
			//TheConsole.Log("2");
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