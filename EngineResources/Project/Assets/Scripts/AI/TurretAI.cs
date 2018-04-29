using TheEngine;

public class TurretAI {
	
	TheGameObject TurretBase = null;	
	public TheGameObject TurretHead	= null;
	TheTransform SelfTransform = null;
	TheVector3 SelfPosition;
	public TheGameObject BlasterPivot = null;
	TheTransform BlasterTransform = null;
	public TheGameObject BlasterCannon = null;
	TheFactory blaster_factory = null;

	public TheGameObject TargetPlayer = null;
	TheTransform PlayerTransform = null;
	TheVector3 PlayerPosition;
	
	float DeltaTime = 0.0f;
	public float MinDistance = 600.0f;
	public float ShootingRange = 500.0f;
	public float MinAngleBlasters = 0.0f;
	public float MaxAngleBlasters = 60.0f;
	
	public float LaserFrequency = 0.100f;
	TheTimer BlasterTimer = new TheTimer();

	void Start () {
		TurretBase = TheGameObject.Self;
		BlasterTransform = BlasterPivot.GetComponent<TheTransform>();
		PlayerTransform = TargetPlayer.GetComponent<TheTransform>();
		SelfTransform = TurretHead.GetComponent<TheTransform>();
		
		blaster_factory	= BlasterCannon.GetComponent<TheFactory>();
		
		BlasterTimer.Start();
	}
	
	void Update () {
		PlayerPosition = PlayerTransform.GlobalPosition;
		SelfPosition = SelfTransform.GlobalPosition;
		DeltaTime = TheTime.DeltaTime;
		
		if (TheVector3.Distance(PlayerPosition, SelfPosition) < MinDistance)
		{
			RotateHeadTowardsPlayer();
			RotateBlasterTowardsPlayer();
			Shoot();
		}	
	}
	
	void RotateHeadTowardsPlayer()
	{
		// Face Forward Dir to PlayerPosition
		
	}
	
	void RotateBlasterTowardsPlayer()
	{
		// Face Blasters to the PlayerPosition
		
	}
	
	void Shoot()
	{
		TheVector3 tOffset = PlayerPosition - SelfPosition;
		
		if (TheVector3.Magnitude(tOffset) < ShootingRange && TheVector3.AngleBetween(SelfTransform.ForwardDirection, tOffset) < MaxAngleBlasters / 2)
		{
			if (BlasterTimer.ReadTime() >= LaserFrequency)
			{
				// 1. Shoot			
				// 2. Change Between Left and Rigth Blasters
				// 3. Restart Timer
				BlasterTimer.Start();
			}
		}
	}
}