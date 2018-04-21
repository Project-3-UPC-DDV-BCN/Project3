using TheEngine;

public class ColliderScalingByDistance {

	TheVector3 min_size = new TheVector3(1f, 1f, 1f);
	public TheVector3 max_size_multiplier = new TheVector3(1f, 1f, 1f);
	TheVector3 max_size = new TheVector3(1f, 1f, 1f);
	public float max_distance = 5000f;

	TheBoxCollider collider = null;

	TheGameObject player = null;
	
	TheTransform transform = null;

	void Start () {
		collider = TheGameObject.Self.GetComponent<TheBoxCollider>();
		if(collider != null) {
			min_size = collider.Size;
		}
		player = GetPlayerFromGM();
		transform = TheGameObject.Self.GetComponent<TheTransform>();
		// Max Size Setup
		max_size.x = min_size.x * max_size_multiplier.x;
		max_size.y = min_size.y * max_size_multiplier.y;
		max_size.z = min_size.z * max_size_multiplier.z;

	}
	
	void Update () {
		if(collider == null) return;
		if(player == null) SecurityReset();
		else {
			collider.Size = GetResizeValue(TheVector3.Distance(transform.GlobalPosition, player.GetComponent<TheTransform>().GlobalPosition));
		}
	}

	void SecurityReset() {
		player = GetPlayerFromGM();
		if(collider != null) collider.Size = min_size;
	}

	TheVector3 GetResizeValue(float player_distance) {
		float percent = player_distance / max_distance;
		return TheVector3.Lerp(min_size, max_size, percent);
	}

	TheGameObject GetPlayerFromGM() {
		TheGameObject GM = TheGameObject.Find("GameManager");
		if(GM != null) {
			TheScript GameManager = GM.GetScript("GameManager");
			if(GameManager != null) {
				return (TheGameObject)GameManager.CallFunctionArgs("GetSlave1");
			}
		}
		return null;
	}

}