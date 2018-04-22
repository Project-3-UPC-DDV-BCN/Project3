using TheEngine;

public class BillboardToPlayer {

	// Scripts ---
	TheScript GameManager = null;
	
	// Transforms
	TheTransform transform;
	TheTransform player_transform;
	TheTransform parent_transform;

	void Start () {
		transform = TheGameObject.Self.GetComponent<TheTransform>();
		TheGameObject GM = TheGameObject.Find("GameManager");
		if(GM != null)
			GameManager = GM.GetScript("GameManager");
		if(GameManager != null) {
			TheGameObject player = (TheGameObject)GameManager.CallFunctionArgs("GetSlave1");
			if(player != null)
				player_transform = player.GetComponent<TheTransform>();
		}
		TheGameObject parent = TheGameObject.Self.GetParent();
		if(parent != null)
			parent_transform = parent.GetComponent<TheTransform>();
	}
	
	void Update () {
		transform.LookAt(player_transform.GlobalPosition);		
		transform.LocalRotation = (transform.LocalRotation - player_transform.GlobalRotation);
	}

}