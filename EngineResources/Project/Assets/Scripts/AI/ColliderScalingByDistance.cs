using TheEngine;

public class ColliderScalingByDistance {

	TheVector3 min_size = new TheVector3(1f, 1f, 1f);
	public TheVector3 max_size_multiplier = new TheVector3(1f, 1f, 1f);
	TheVector3 max_size = new TheVector3(1f, 1f, 1f);
	public float max_distance = 5000f;

	TheBoxCollider collider = null;

	TheGameObject player = null;
	TheTransform player_transform = null;
	
	TheTransform transform = null;
	
	TheGameObject self = TheGameObject.Self;
	
	TheGameObject GM = null;

	void Init()
	{
		
		GM = TheGameObject.Find("GameManager");

		collider = self.GetComponent<TheBoxCollider>();

		player = GetPlayerFromGM();

		if(player != null)
			player_transform = player.GetComponent<TheTransform>();

		transform = self.GetComponent<TheTransform>();
	}

	void Start () 
	{
		if(collider != null) 
			min_size = collider.Size;
		
		player = GetPlayerFromGM();

		// Max Size Setup
		max_size.x = min_size.x * max_size_multiplier.x;
		max_size.y = min_size.y * max_size_multiplier.y;
		max_size.z = min_size.z * max_size_multiplier.z;

	}
	
	void Update () 
	{
		if(collider == null) 
			return;

		if(player == null || player_transform == null)
		{ 
			SecurityReset();

			if(player != null)
				player_transform = player.GetComponent<TheTransform>();
		}
		else 
			collider.Size = GetResizeValue(TheVector3.Distance(transform.GlobalPosition, player_transform.GlobalPosition));
		
	}

	void SecurityReset() 
	{
		player = GetPlayerFromGM();

		if(collider != null)
			 collider.Size = min_size;
	}

	TheVector3 GetResizeValue(float player_distance) 
	{
		TheVector3 ret = new TheVector3(0, 0, 0);

		if(collider != null)
		{
			ret = collider.Size;

			if(max_distance != 0)
			{
        		if (player_distance > max_distance) 
					player_distance = max_distance;

				float percent = player_distance / max_distance;

				ret = TheVector3.Lerp(min_size, max_size, percent);
			}
		}

		return ret;
	}

	TheGameObject GetPlayerFromGM() 
	{
		if(GM != null) 
		{
			TheScript GameManager = GM.GetScript("GameManager");

			if(GameManager != null) 
			{
				return (TheGameObject)GameManager.CallFunctionArgs("GetSlave1");
			}
		}

		return null;
	}

}