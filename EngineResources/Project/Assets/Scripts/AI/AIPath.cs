using TheEngine;
using System.Collections.Generic; 
using TheEngine.TheConsole;
using TheEngine.TheMath;

public class AIPath {

	public TheGameObject PathGroup = null;
	public bool IsPatrol = false;
	
	float min_dist = 1.0f;
	
	TheGameObject Self = TheGameObject.Self;
	TheTransform SelfTransform;
	
	List<TheGameObject> Nodes = new List<TheGameObject>();
	List<TheVector3> NodePosition = new List<TheVector3>();
	
	public float move_speed = 50;
    public float rotation_speed = 60;
	float modified_move_speed = 50;
    float modified_rotation_speed = 60;
	
	TheVector3 next_node = null;
	int node_index = 0;
	int num_nodes = 0;
	
	float DeltaTime = 0.0f;
	
	void Start () {
		SelfTransform = TheGameObject.Self.GetComponent<TheTransform>();
		
		TheGameObject[] nodes = PathGroup.GetAllChilds();
		foreach (TheGameObject node in nodes)
		{
			Nodes.Add(node);
			NodePosition.Add(node.GetComponent<TheTransform>().GlobalPosition);
			TheConsole.Log("Node Position: "+node.GetComponent<TheTransform>().GlobalPosition.x+","
											+node.GetComponent<TheTransform>().GlobalPosition.y+","
											+node.GetComponent<TheTransform>().GlobalPosition.z);
			//TheConsole.Log("Name: "+ node.name);
		}
		//TheConsole.Log("Total Nodes: "+Nodes.Count);
		num_nodes = Nodes.Count;
		node_index = 0;
		next_node = NodePosition[0];
	}
	
	void Update () {
		
		DeltaTime = TheTime.DeltaTime;
		
		if (TheVector3.Distance(SelfTransform.GlobalPosition, next_node) > min_dist)
		{
			// Move
			if (next_node != null)
			{
				MoveFront();
				OrientateToTarget();
			}
		}
		else
		{
			if (node_index < num_nodes - 1)
			{
				node_index++;
				next_node = NodePosition[node_index];
			}
			else if (IsPatrol)
			{
				List<TheVector3> new_list = new List<TheVector3>();
				
				for (int i = num_nodes - 1; i >= 0; i--)
					new_list.Add(NodePosition[i]);
				
				NodePosition.Clear();
				
				foreach (TheVector3 node in new_list)
					NodePosition.Add(node);
					
				node_index = 1;
				next_node = NodePosition[node_index];
			}
		}
		
	}
	
	void MoveFront()
    {
        if (next_node != null)
        {
            TheVector3 pos = SelfTransform.LocalPosition;
			TheVector3 forward_dir = SelfTransform.ForwardDirection;

            TheVector3 move = new TheVector3(0, 0, 0);
            move.x = move_speed * forward_dir.x;
            move.y = move_speed * forward_dir.y;
            move.z = move_speed * forward_dir.z;

            SelfTransform.LocalPosition = new TheVector3(pos.x + (move.x * DeltaTime), pos.y + (move.y * DeltaTime), pos.z + (move.z * DeltaTime));
        }
    }
	
	void OrientateToTarget()
    {
		TheVector3 self_pos = SelfTransform.GlobalPosition;
        TheVector3 target_pos = next_node;	
		TheVector3 self_trans_rot = SelfTransform.LocalRotation;

        float target_x_angle = -GetAngleFromTwoPoints(self_pos.x, self_pos.z, target_pos.x, target_pos.z) - 270;

        float angle_diff_x = self_trans_rot.y - target_x_angle;
	
		if((NormalizeAngle(self_trans_rot.x) < 90 && NormalizeAngle(self_trans_rot.x) > -90) || (NormalizeAngle(self_trans_rot.x) > 270 || NormalizeAngle(self_trans_rot.x) < -270))
		{
        	if (NormalizeAngle(angle_diff_x) > 180)
            	SelfTransform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y - (modified_rotation_speed * DeltaTime), self_trans_rot.z);
        	else
           	 	SelfTransform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y + (modified_rotation_speed * DeltaTime), self_trans_rot.z);
		}
		else
		{
			if (NormalizeAngle(angle_diff_x) > 0)
				SelfTransform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y - (modified_rotation_speed * DeltaTime), self_trans_rot.z);
        	else
           	 	SelfTransform.LocalRotation = new TheVector3(self_trans_rot.x, self_trans_rot.y + (modified_rotation_speed * DeltaTime), self_trans_rot.z);
		}
		
		self_trans_rot = SelfTransform.LocalRotation;
			
		float target_y_angle = GetAngleFromTwoPoints(self_pos.y, self_pos.z, target_pos.y, target_pos.z) - 270;

		if(self_pos.z > target_pos.z)
		{
			target_y_angle -= 180;
		}		

        float angle_diff_y =self_trans_rot.x - target_y_angle;		

		if (NormalizeAngle(angle_diff_y) > 180)
			SelfTransform.LocalRotation = new TheVector3(self_trans_rot.x + (modified_rotation_speed * DeltaTime), self_trans_rot.y, self_trans_rot.z);
		else
			SelfTransform.LocalRotation = new TheVector3(self_trans_rot.x - (modified_rotation_speed * DeltaTime), self_trans_rot.y, self_trans_rot.z);
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
}