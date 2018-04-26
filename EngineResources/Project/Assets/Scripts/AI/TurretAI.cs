using TheEngine;

public class TurretAI {
	
	TheGameObject TurretBase = null;	
	public TheGameObject TurretHead	= null;
	public TheGameObject BlasterPivot = null;
	public TheGameObject BlasterCannon = null;
	TheFactory blaster_factory = null;	

	void Start () {
		TurretBase = TheGameObject.Self;
		blaster_factory	= BlasterCannon.GetComponent<TheFactory>();
	}
	
	void Update () {
		
	}
}