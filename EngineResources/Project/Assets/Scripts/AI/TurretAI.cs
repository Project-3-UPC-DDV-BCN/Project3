using TheEngine;
using TheEngine.TheConsole;

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
	
	TheAudioSource AudioSource = null;
	
	float DeltaTime = 0.0f;
	public float MinDistance = 600.0f;
	public float ShootingRange = 500.0f;
	public float MinAngleBlasters = 0.0f;
	public float MaxAngleBlasters = 60.0f;
	
	public float LaserFrequency = 0.100f;
	public float LaserSpeed = 30.0f;
	public int BaseLaserDamage = 10;
	
	TheTimer BlasterTimer = new TheTimer();

	void Start () {
		TurretBase = TheGameObject.Self;
		BlasterTransform = BlasterPivot.GetComponent<TheTransform>();
		PlayerTransform = TargetPlayer.GetComponent<TheTransform>();
		SelfTransform = TurretHead.GetComponent<TheTransform>();
		
		blaster_factory	= BlasterCannon.GetComponent<TheFactory>();
		
		AudioSource = TurretBase.GetComponent<TheAudioSource>();
		
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
		
		//if (TheVector3.Magnitude(tOffset) < ShootingRange && TheVector3.AngleBetween(SelfTransform.ForwardDirection, tOffset) < MaxAngleBlasters / 2)
		if (TheInput.IsKeyRepeat("UP_ARROW"))
		{
			TheConsole.Warning(BlasterTimer.ReadTime());
			if (BlasterTimer.ReadTime() >= LaserFrequency && blaster_factory != null)
			{
				TheGameObject laser = blaster_factory.Spawn();
				
				TheConsole.Warning("1");
				
				if(laser != null)
				{
					TheConsole.Warning("2");
					if(AudioSource != null)
						AudioSource.Play("Play_Shoot");
					
					TheScript laser_script = laser.GetScript("Laser");

					if(laser_script != null)
					{
						object[] args = {SelfTransform, LaserSpeed, BaseLaserDamage, BlasterTransform.ForwardDirection, BlasterTransform.QuatRotation};
						laser_script.CallFunctionArgs("SetInfo", args);
					}
				}
				// 1. Shoot			
				// 2. Change Between Left and Rigth Blasters
				// 3. Restart Timer
				//BlasterTimer.Start();
			}
		}
	}
}