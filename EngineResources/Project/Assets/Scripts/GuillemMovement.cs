using TheEngine;
using System.Collections.Generic;
using TheEngine.TheMath;
using TheEngine.TheConsole;

public class GuillemMovement 
{
	TheTransform self_transform = null;
		
	TheTransform target_transform = null;

	public float move_speed = 100;
	public float rotation_speed = 100;
	
	void Start () 
	{
		self_transform = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () 
	{
		if(target_transform == null)
			LookForTarget();

		if(target_transform != null)
		{
			MoveFront();
			OrientateToTarget();
		}
	}

	void LookForTarget()
	{
		TheGameObject[] scene_gos = TheGameObject.GetSceneGameObjects();
		for(int i = 0; i < scene_gos.Length; ++i)
		{
			if(scene_gos[i].name == "XWingTarget")
			{
				target_transform = scene_gos[i].GetComponent<TheTransform>();
				TheConsole.Log("FoundTarget!");
				break;
			}
		}
	}

	void MoveFront()
	{
		if(target_transform != null)
		{
			TheVector3 pos = self_transform.LocalPosition;
			TheVector3 move = new TheVector3(0, 0, 0);
			move.x = move_speed * self_transform.ForwardDirection.x;
			move.y = move_speed * self_transform.ForwardDirection.y;
			move.z = move_speed * self_transform.ForwardDirection.z;
			self_transform.LocalPosition = new TheVector3(pos.x + (move.x * TheTime.DeltaTime), pos.y + (move.y * TheTime.DeltaTime), pos.z + (move.z * TheTime.DeltaTime));
		}
	}

	void OrientateToTarget()
	{
		TheVector3 self_pos = self_transform.GlobalPosition;
		TheVector3 target_pos = target_transform.GlobalPosition;
		
		// x
		float target_x_angle = -GetAngleFromTwoPoints(self_pos.x, self_pos.z, target_pos.x, target_pos.z) - 270;
		
		float angle_diff_x = self_transform.LocalRotation.y - target_x_angle;
		TheConsole.Log(angle_diff_x);

		if(NormalizeAngle(angle_diff_x) > 180)
			self_transform.LocalRotation = new TheVector3(self_transform.LocalRotation.x, self_transform.LocalRotation.y - (rotation_speed * TheTime.DeltaTime), self_transform.LocalRotation.z);
		else
			self_transform.LocalRotation = new TheVector3(self_transform.LocalRotation.x, self_transform.LocalRotation.y + (rotation_speed * TheTime.DeltaTime), self_transform.LocalRotation.z);

		// y
		float target_y_angle = GetAngleFromTwoPoints(self_pos.y, self_pos.z, target_pos.y, target_pos.z) + 270;
		
		float angle_diff_y = self_transform.LocalRotation.x - target_y_angle;

		if(target_pos.z > self_pos.z)
			angle_diff_y += 180;
		TheConsole.Log(angle_diff_y);

		if(NormalizeAngle(angle_diff_y) < 180 && NormalizeAngle(angle_diff_y) > -180)
		{
			if(NormalizeAngle(angle_diff_y) > 0)
				self_transform.LocalRotation = new TheVector3(self_transform.LocalRotation.x - (rotation_speed * TheTime.DeltaTime), self_transform.LocalRotation.y, self_transform.LocalRotation.z);
			else
				self_transform.LocalRotation = new TheVector3(self_transform.LocalRotation.x + (rotation_speed * TheTime.DeltaTime), self_transform.LocalRotation.y, self_transform.LocalRotation.z);
		
			if(self_transform.LocalRotation.x > 180 || self_transform.LocalRotation.x < -180)
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
}