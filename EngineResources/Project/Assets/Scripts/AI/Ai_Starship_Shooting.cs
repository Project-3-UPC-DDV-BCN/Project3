using TheEngine;
//using TheEngine.TheConsole; 

public class Ai_Starship_Shooting : TheObject
{
	public float time_between_lasers = 0.100f;
	public float shooting_range = 500f;
    public float shooting_angle = 60f;

	bool shooting = false;

	TheGameObject slave1 = null;
	TheTransform slave1_transform = null;

	TheTransform transform = null;
	TheTransform target_transform = null;

	bool in_range = false;

	// Scripts ---
	TheScript movement = null;
	TheScript game_manager_script = null;
	TheScript ship_properties = null;

	TheTimer timer = new TheTimer();
	
	public override void Init()
	{
		TheGameObject game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");

		ship_properties = Self.GetScript("ShipProperties");

		transform = Self.GetComponent<TheTransform>();

	}
	
	public override void Start () 
	{
		if(game_manager_script != null)
			slave1 = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");	

		if(slave1 != null)
			slave1_transform = slave1.GetComponent<TheTransform>();

		timer.Start();
	}
	
	public override void Update () 
	{
		CheckForShooting();
	}

	void CheckForShooting()
	{
		if(slave1_transform != null && transform != null)
		{
			float mag_distance = TheVector3.Distance(slave1_transform.GlobalPosition, transform.GlobalPosition);

			if(mag_distance < 50000)
			{
				in_range = true;
			}
			else
			{
				in_range = false;
			}
		}
	
		if(target_transform != null && transform != null) 
		{	
			TheVector3 tOffset = target_transform.GlobalPosition - transform.GlobalPosition;

			if(TheVector3.Magnitude(tOffset) < shooting_range && TheVector3.AngleBetween(transform.ForwardDirection, tOffset) < shooting_angle / 2)	
				shooting = true;	
			else
				shooting = false;
			
			if(shooting) 
			{
				if(timer.ReadTime() >= time_between_lasers) 
				{
					if(ship_properties != null)
						ship_properties.CallFunctionArgs("Shoot");

					timer.Start();
				}
			}
		}
	}

	void SetTargetTransform(TheTransform trans)
	{
		target_transform = trans;

		//TheConsole.Log("Shooting target set!");
	}

	void ClearTargetTransform()
	{
		target_transform = null;
		shooting = false;
		transform = null;
	}
}