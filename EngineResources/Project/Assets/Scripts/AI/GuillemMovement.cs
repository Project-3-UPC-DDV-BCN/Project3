using TheEngine;
using System.Collections.Generic;
using TheEngine.TheMath;
using TheEngine.TheConsole;

public class GuillemMovement
{
    public int faction;
    public TheGameObject force_target;
    private bool forced = false;

    TheGameObject center_object;
    public float max_distance_to_center_object = 2000f;
    public float desertor_distance = 10000f;

    public float move_speed = 300;
    public float rotation_speed = 60;

    private float modified_move_speed = 0;
    private float modified_rotation_speed = 0;

    TheTransform self_transform = null;
    TheTransform target_transform = null;
    TheTransform center_transform = null;

    public TheVector3 target_pos;
	public float target_pos_x = 0;
	public float target_pos_y = 0;
	public float target_pos_z = 0;

    TheTimer timer = new TheTimer();
    float random_time = 0;
	
	// Scripts ---
	TheScript ShipProperties = null;
	TheScript GameManager = null;

	// Self Scripts Keys ---
	public int ShipPropertiesKey = 0;


    void Start()
    {

		ShipProperties = TheGameObject.Self.GetComponent<TheScript>(ShipPropertiesKey);
		TheGameObject GM = TheGameObject.Find("GameManager");
		GameManager = GM.GetComponent<TheScript>();

        RandomizeStats();

        self_transform = TheGameObject.Self.GetComponent<TheTransform>();

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

    }

    void Update()
    {
        // Change target after x seconds
        if (timer.ReadTime() > random_time && !forced)
        {
            target_transform = null;
            timer.Start();
            RandomizeStats();
        }

        // Avoid leaving x point on the map 
        if (center_transform != null)
        {
            if (TheVector3.Distance(center_transform.GlobalPosition, self_transform.GlobalPosition) > desertor_distance)
                TheGameObject.Destroy(TheGameObject.Self);

            if (TheVector3.Distance(center_transform.GlobalPosition, self_transform.GlobalPosition) > max_distance_to_center_object)
            {
                target_transform = center_transform;
            }
            else if (TheVector3.Distance(center_transform.GlobalPosition, self_transform.GlobalPosition) < max_distance_to_center_object / 2 &&
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
            target_pos = target_transform.GlobalPosition;
			target_pos_x = target_pos.x;
			target_pos_y = target_pos.y;
			target_pos_z = target_pos.z;
        }
    }

    void LookForTarget()
    {

		if(ShipProperties == null) return;

		string factionStr = (string)ShipProperties.CallFunctionArgs("GetFaction");

		List<TheGameObject> enemy_ships = new List<TheGameObject>();	

		if(factionStr == "alliance") {
			enemy_ships = (List<TheGameObject>)GameManager.CallFunctionArgs("GetEmpireShips");
		}
		else if(factionStr == "empire") {
			enemy_ships = (List<TheGameObject>)GameManager.CallFunctionArgs("GetAllianceShips");
		}		

		TheGameObject PlayerGo = (TheGameObject)GameManager.CallFunctionArgs("GetSlave1");
		
		if(PlayerGo != null) {
			// Player faction Filter Here ...
			// ...
			enemy_ships.Add(PlayerGo);
		}

		if(enemy_ships.Count > 0) {
			int random = (int)TheRandom.RandomRange(0, enemy_ships.Count);
			target_transform = enemy_ships[random].GetComponent<TheTransform>();
		}

        /*List<TheGameObject> to_check = new List<TheGameObject>();
        TheGameObject[] scene_gos = TheGameObject.GetSceneGameObjects();
        for (int i = 0; i < scene_gos.Length; ++i)
        {
            if (scene_gos[i] != null)
            {
				if(scene_gos[i].tag == "PlayerTarget") {
					to_check.Add(scene_gos[i]);
					continue;
				}
                if (scene_gos[i].tag == "XWING" || scene_gos[i].tag == "TIEFIGHTER")
                {
                    to_check.Add(scene_gos[i]);
                    TheScript sc = scene_gos[i].GetComponent<TheScript>();
                    if (sc != null)
                    {
                        int other_fac = sc.GetIntField("faction");

                        if (other_fac != faction)
                        {
                            to_check.Add(scene_gos[i]);
                        }
                    }
                }
            }
        }

        if (to_check.Count > 0)
        {
            int random = (int)TheRandom.RandomRange(0, to_check.Count);
            target_transform = to_check[random].GetComponent<TheTransform>();
        }*/
    }

    void MoveFront()
    {
        if (target_transform != null)
        {
            TheVector3 pos = self_transform.LocalPosition;
            TheVector3 move = new TheVector3(0, 0, 0);
            move.x = modified_move_speed * self_transform.ForwardDirection.x;
            move.y = modified_move_speed * self_transform.ForwardDirection.y;
            move.z = modified_move_speed * self_transform.ForwardDirection.z;
            self_transform.LocalPosition = new TheVector3(pos.x + (move.x * TheTime.DeltaTime), pos.y + (move.y * TheTime.DeltaTime), pos.z + (move.z * TheTime.DeltaTime));
        }
    }

    void OrientateToTarget()
    {
        TheVector3 self_pos = self_transform.GlobalPosition;
        TheVector3 target_pos = target_transform.GlobalPosition;

        // x
        float target_x_angle = -GetAngleFromTwoPoints(self_pos.x, self_pos.z, target_pos.x, target_pos.z + 30) - 270;

        float angle_diff_x = self_transform.LocalRotation.y - target_x_angle;

        if (NormalizeAngle(angle_diff_x) > 180)
            self_transform.LocalRotation = new TheVector3(self_transform.LocalRotation.x, self_transform.LocalRotation.y - (modified_rotation_speed * TheTime.DeltaTime), self_transform.LocalRotation.z);
        else
            self_transform.LocalRotation = new TheVector3(self_transform.LocalRotation.x, self_transform.LocalRotation.y + (modified_rotation_speed * TheTime.DeltaTime), self_transform.LocalRotation.z);

        // y
        float target_y_angle = GetAngleFromTwoPoints(self_pos.y, self_pos.z, target_pos.y, target_pos.z + 30) + 270;

        float angle_diff_y = self_transform.LocalRotation.x - target_y_angle;

        if (target_pos.z > self_pos.z)
            angle_diff_y += 180;

        if (NormalizeAngle(angle_diff_y) < 180 && NormalizeAngle(angle_diff_y) > -180)
        {
            if (NormalizeAngle(angle_diff_y) > 0)
                self_transform.LocalRotation = new TheVector3(self_transform.LocalRotation.x - (modified_rotation_speed * TheTime.DeltaTime), self_transform.LocalRotation.y, self_transform.LocalRotation.z);
            else
                self_transform.LocalRotation = new TheVector3(self_transform.LocalRotation.x + (modified_rotation_speed * TheTime.DeltaTime), self_transform.LocalRotation.y, self_transform.LocalRotation.z);

            if (self_transform.LocalRotation.x > 180 || self_transform.LocalRotation.x < -180)
                self_transform.LocalRotation = new TheVector3(0, self_transform.LocalRotation.y, self_transform.LocalRotation.z);
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