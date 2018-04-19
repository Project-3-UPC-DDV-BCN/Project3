using TheEngine;

public class Ai_Starship_Shooting 
{
	TheAudioSource audio_source = null;

	public float time_between_lasers = 0.100f;

	float timer = 0.0f;

	public bool shooting = false;

	TheTransform transform = null;

	public float shooting_range = 500f;
    public float shooting_angle = 60f;

	TheGameObject player = null;

	bool in_range = false;

	// Scripts ---
	TheScript GameManager = null;
	TheScript movement = null;
	TheScript ShipProperties = null;

	void Start () 
	{
		ShipProperties = TheGameObject.Self.GetScript("ShipProperties");
		movement = TheGameObject.Self.GetScript("GuillemMovement");
		transform = TheGameObject.Self.GetComponent<TheTransform>();
		audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();

		TheGameObject GM = TheGameObject.Find("GameManager");
		if(GM != null)
			GameManager = GM.GetComponent<TheScript>("GameManager");
		if(GameManager != null)
			player = (TheGameObject)GameManager.CallFunctionArgs("GetPlayer");	

	}
	
	void Update () 
	{
		if(player != null)
		{
			TheTransform player_trans = player.GetComponent<TheTransform>();
			if(player_trans != null)
			{
				float mag_distance = TheVector3.Distance(player_trans.GlobalPosition, transform.GlobalPosition);

				if(mag_distance < 50000)
				{
					in_range = true;
				}
				else
				{
					in_range = false;
				}
			}
		}
	
		if(movement != null) 
		{
			TheVector3 auxTPos = TheVector3.Zero;
			auxTPos.x = movement.GetFloatField("target_pos_x");
			auxTPos.y = movement.GetFloatField("target_pos_y");
			auxTPos.z = movement.GetFloatField("target_pos_z");
			TheVector3 tOffset = auxTPos - transform.GlobalPosition;
			if(TheVector3.Magnitude(tOffset) < shooting_range && TheVector3.AngleBetween(transform.ForwardDirection, tOffset) < shooting_angle / 2)	
				shooting = true;	
			else
			{
				shooting = false;
			}
		}

		if(shooting) 
		{
			timer += TheTime.DeltaTime;
			if(timer >= time_between_lasers) 
			{
				if(ShipProperties != null)
					ShipProperties.CallFunctionArgs("Shoot");
				timer = 0.0f;
			}
			if(in_range) 
			{
				audio_source.Stop("Play_Shoot");
				audio_source.Play("Play_Shoot");
			}
		}
	}
}