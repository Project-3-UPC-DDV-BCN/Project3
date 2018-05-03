using TheEngine;
using TheEngine.TheConsole;
using TheEngine.TheMath;

public class TurretAI {
	
	TheGameObject TurretBase = null;	
	public TheGameObject TurretHead	= null;
	TheTransform SelfTransform = null;
	TheVector3 SelfPosition;
	TheVector3 SelfRotation;
	public TheGameObject BlasterPivot = null;
	TheTransform BlasterTransform = null;
	TheVector3 BlasterPosition;
	TheVector3 BlasterRotation;
	public TheGameObject BlasterCannon = null;
	TheTransform CannonTransform = null;
	TheFactory blaster_factory = null;

	public TheGameObject TargetPlayer = null;
	TheTransform PlayerTransform = null;
	TheVector3 PlayerPosition;
	
	TheAudioSource AudioSource = null;
	
	float DeltaTime = 0.0f;
	public float RotationSpeed = 50.0f;
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
		CannonTransform = BlasterCannon.GetComponent<TheTransform>();
		
		blaster_factory	= BlasterCannon.GetComponent<TheFactory>();
		blaster_factory.StartFactory();
		
		AudioSource = TurretBase.GetComponent<TheAudioSource>();
		
		BlasterTimer.Start();
	}
	
	void Update () {
		PlayerPosition = PlayerTransform.GlobalPosition;
		SelfPosition = SelfTransform.GlobalPosition;
		SelfRotation = SelfTransform.LocalRotation;
		BlasterPosition = BlasterTransform.GlobalPosition;
		BlasterRotation = BlasterTransform.LocalRotation;
		
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
		TheVector3 LookPos = new TheVector3(PlayerPosition.x - SelfPosition.x, 0, PlayerPosition.z - SelfPosition.z);
		TheQuaternion q = TheQuaternion.LookRotation(LookPos, SelfTransform.UpDirection);
		SelfTransform.QuatRotation = TheQuaternion.Slerp(SelfTransform.QuatRotation, q, DeltaTime * RotationSpeed);
	}
	
	void RotateBlasterTowardsPlayer()
	{
		// Face Blasters to the PlayerPosition
		/*TheVector3 LookPos = new TheVector3(0, PlayerPosition.y - BlasterPosition.y, PlayerPosition.z - BlasterPosition.z);
		TheQuaternion q = TheQuaternion.LookRotation(LookPos, CannonTransform.UpDirection);
		BlasterTransform.QuatRotation = TheQuaternion.Slerp(BlasterTransform.QuatRotation, q, DeltaTime * RotationSpeed);
	
		if (BlasterRotation.x < MinAngleBlasters)
			BlasterRotation = new TheVector3(MinAngleBlasters, 0, 0);
		else if (BlasterRotation.x > MaxAngleBlasters)
			BlasterRotation = new TheVector3(MaxAngleBlasters, 0, 0);
		*/
	
	}
	
	void Shoot()
	{
		TheVector3 tOffset = PlayerPosition - SelfPosition;
		
		//if (TheVector3.Magnitude(tOffset) < ShootingRange && TheVector3.AngleBetween(SelfTransform.ForwardDirection, tOffset) < MaxAngleBlasters / 2)
		if (TheInput.IsKeyRepeat("UP_ARROW"))
		{
			if (BlasterTimer.ReadTime() >= LaserFrequency && blaster_factory != null)
			{
				TheGameObject laser = blaster_factory.Spawn();
				
				if(laser != null)
				{
					TheConsole.Warning("2");
					if(AudioSource != null)
						AudioSource.Play("Play_Shoot");
					
					TheScript laser_script = laser.GetScript("Laser");

					if(laser_script != null)
					{
						object[] args = {CannonTransform, LaserSpeed, BaseLaserDamage, CannonTransform.ForwardDirection, CannonTransform.QuatRotation};
						laser_script.CallFunctionArgs("SetInfo", args);
					}
				}
				// 1. Shoot			
				// 2. Change Between Left and Rigth Blasters
				// 3. Restart Timer
				BlasterTimer.Start();
			}
		}
	}
	
	float GetAngleFromTwoPoints(float x1, float y1, float x2, float y2)
    {
        float xDiff = x1 - x2;
        float yDiff = y1 - y2;
        return (float)TheMath.Atan2(yDiff, xDiff) * (float)(180 / TheMath.PI);
    }

    float NormalizeAngle(float angle)
    {
        if (angle > 360)
        {
            int multiplers = (int)(angle / 360);
            angle -= (360 * multiplers);
        }

        if (angle < -360)
        {
            int multiplers = (int)(System.Math.Abs(angle) / 360);
            angle += (360 * multiplers);
        }

        return angle;
    }
}