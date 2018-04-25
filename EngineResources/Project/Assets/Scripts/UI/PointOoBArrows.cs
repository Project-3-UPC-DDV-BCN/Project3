using TheEngine;
using TheEngine.TheMath;
using System.Collections.Generic;

public class PointOoBArrows : TheObject {

	public TheGameObject arrow01;
	public float radius = 1f;

	public float activeArrowThresholdX = 640f;
	public float activeArrowThresholdY = 480f;

	public float arrowAngleOffset = 90f;

	TheTransform transform;

	public bool perspective2d = false;

	public float debugX = 0f;
	public float debugY = 0f;

	public TheVector3 original_pos = new TheVector3();

	// Transforms ---
	TheTransform player_transform = null;

    // Scripts ---
    TheScript GameManager = null;

	public override void Start () {
		transform = Self.GetComponent<TheTransform>();
        TheGameObject GM = TheGameObject.Find("GameManager");
        if (GM != null)
            GameManager = GM.GetScript("GameManager");
		if(GameManager != null) {
			TheGameObject player = (TheGameObject)GameManager.CallFunctionArgs("GetSlave1");
			if(player != null)
				player_transform = player.GetComponent<TheTransform>();
		}
	}
	
	public override void Update () {
        List<TheGameObject> enemies = new List<TheGameObject>();
        // For the moment I will assume Slave1 is ALWAYS ALLIANCE --- Change When Slave1 has a faction defined in code
        // Slave1 faction filter here ...
        // ...
        enemies = (List<TheGameObject>)GameManager.CallFunctionArgs("GetEmpireShips");
        // Since we have no target, I will put the Arrow pointing to the first enemy inside the list --- Will require Changing and/or polish ---
        if (enemies.Count > 0) MoveArrowToPosition(enemies[0], arrow01);
        else arrow01.SetActive(false);
	}

	void MoveArrowToPosition(TheGameObject ship, TheGameObject arrow) {
		if(ship == null || arrow == null || player_transform == null) return;	

		// Is Ship in Frustrum ? arrow.active = true : arrow.active = false
		//	...
		
		
		//if(arrow.IsActive() == false || ship.IsActive() == false) return;		

		TheRectTransform arrowTrans = arrow.GetComponent<TheRectTransform>();
		TheTransform ship_transform = ship.GetComponent<TheTransform>();
		TheVector3 dv2 = ship_transform.GlobalPosition - player_transform.GlobalPosition;
		//dv2 = Coord3DtoCoord2D(dv2, perspective2d);
		if(TheMath.Abs(dv2.x) > activeArrowThresholdX || TheMath.Abs(dv2.y) > activeArrowThresholdY) {
			arrow.SetActive(true);
		}
		else {
			arrow.SetActive(false);
		}
		//dv2.x = -dv2.x;
		debugX = dv2.x;
		debugY = dv2.y;
		arrowTrans.Position = original_pos + dv2.Normalized * radius;
		if(dv2.x > 0) arrowTrans.Rotation = new TheVector3(0, 0,-(arrowAngleOffset + TheMath.RadToDeg * TheMath.Atan(dv2.y / -dv2.x)));
		else arrowTrans.Rotation = new TheVector3(0, 0, arrowAngleOffset + TheMath.RadToDeg * TheMath.Atan2(dv2.x, dv2.y));
	}

	TheVector3 Coord3DtoCoord2D(TheVector3 v3d, bool w_perspective = false) {
		if(w_perspective)
			return new TheVector3(v3d.x/v3d.z, v3d.y/v3d.z, 0f).Normalized * TheVector3.Magnitude(v3d);
		return new TheVector3(v3d.x, v3d.y, 0f);
	}

}