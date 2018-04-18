using TheEngine;
using TheEngine.TheMath;

public class PointOoBArrows {

	public TheGameObject arrow01;
	public float radius = 1f;

	public float activeArrowThresholdX = 640f;
	public float activeArrowThresholdY = 480f;

	public float arrowAngleOffset = 90f;

	TheTransform transform;

	public bool perspective2d = false;

	public float debugX = 0f;
	public float debugY = 0f;

	void Start () {
		transform = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		TheGameObject[] scene_gos = TheGameObject.GetSceneGameObjects();
		for(int i = 0; i < scene_gos.Length; i++) {
			if(isShip(scene_gos[i])) {
				MoveArrowToPosition(scene_gos[i], arrow01);	// Which ships you want to point at ?
			}
		}
	}

	void MoveArrowToPosition(TheGameObject ship, TheGameObject arrow) {
		if(ship == null || arrow == null) return;	

		// Is Ship in Frustrum ? arrow.active = true : arrow.active = false
		//	...
		
		
		//if(arrow.IsActive() == false || ship.IsActive() == false) return;		

		TheRectTransform arrowTrans = arrow.GetComponent<TheRectTransform>();
		TheTransform ship_transform = ship.GetComponent<TheTransform>();
		TheVector3 dv2 = ship_transform.GlobalPosition - (transform.GlobalPosition + transform.ForwardDirection.Normalized);
		dv2 = Coord3DtoCoord2D(dv2, perspective2d);
		if(TheMath.Abs(dv2.x) > activeArrowThresholdX || TheMath.Abs(dv2.y) > activeArrowThresholdY) {
			arrow.SetActive(true);
		}
		else {
			arrow.SetActive(false);
		}
		dv2.x = -dv2.x;
		debugX = dv2.x;
		debugY = dv2.y;
		arrowTrans.Position = dv2.Normalized * radius;
		if(dv2.x > 0) arrowTrans.Rotation = new TheVector3(0, 0,-(arrowAngleOffset + TheMath.RadToDeg * TheMath.Atan(dv2.y / -dv2.x)));
		else arrowTrans.Rotation = new TheVector3(0, 0, arrowAngleOffset + TheMath.RadToDeg * TheMath.Atan(dv2.y / dv2.x));
	}

	TheVector3 Coord3DtoCoord2D(TheVector3 v3d, bool w_perspective = false) {
		if(w_perspective)
			return new TheVector3(v3d.x/v3d.z, v3d.y/v3d.z, 0f).Normalized * TheVector3.Magnitude(v3d);
		return new TheVector3(v3d.x, v3d.y, 0f);
	}
	bool isShip(TheGameObject go) {
		if(go == null) return false;
		if(go.tag == "XWING" || go.tag == "TIEFIGHTER" || go.tag == "LANDCRAFTING" || go.tag == "YWING")
			return true;
		return false;
	}

}