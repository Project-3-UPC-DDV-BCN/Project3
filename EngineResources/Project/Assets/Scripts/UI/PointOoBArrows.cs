using TheEngine;

public class PointOoBArrows {

	public TheGameObject arrow01;
	public float radius = 1f;

	TheTransform transform;

	void Start () {
		transform = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		TheGameObject[] scene_gos = TheGameObject.GetSceneGameObjects();
		for(int i = 0; i < scene_gos.Length; i++) {
			if(isShip(scene_gos[i])) {
				MoveArrowToPosition(scene_gos[i], arrow01);	// Which ships ypu want to point at ?
			}
		}
	}

	void MoveArrowToPosition(TheGameObject ship, TheGameObject arrow) {
		if(ship == null || arrow == null) return;	

		// Is Ship in Frustrum ? arrow.active = true : arrow.active = false
		//	...
		
		if(arrow.IsActive() == false || ship.IsActive() == false) return;		

		TheRectTransform arrowTrans = arrow.GetComponent<TheRectTransform>();
		TheTransform ship_transform = ship.GetComponent<TheTransform>();
		TheVector3 dv2 = ship_transform.GlobalPosition - (transform.GlobalPosition + transform.ForwardDirection.Normalized);
		dv2 = Coord3DtoCoord2D(dv2, false);
		arrowTrans.Position = dv2.Normalized * radius;
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