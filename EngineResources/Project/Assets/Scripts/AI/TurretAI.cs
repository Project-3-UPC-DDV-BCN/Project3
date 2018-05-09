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
	public TheGameObject LBlaster = null;
	TheTransform LBlasterTransform = null;
	TheFactory LBlasterFactory = null;
	public TheGameObject RBlaster = null;
	TheTransform RBlasterTransform = null;
	TheFactory RBlasterFactory = null;
	bool shoot = false;
	
	TheGameObject TargetPlayer = null;
	TheTransform PlayerTransform = null;
	TheVector3 PlayerPosition;
	TheGameObject game_manager = null;
	TheScript game_manager_script = null;
	
	TheAudioSource AudioSource = null;
	
	float DeltaTime = 0.0f;
	public float RotationSpeed = 50.0f;
	public float BlasterRotationSpeed = 30.0f;
	public float MinDistance = 600.0f;
	public float ShootingRange = 500.0f;
	public float MinAngleBlasters = 0.0f;
	public float MaxAngleBlasters = 60.0f;
	
	public float LaserFrequency = 0.100f;
	public float LaserSpeed = 30.0f;
	public int BaseLaserDamage = 10;
	
	public float AngleOffset = 8.0f;
	
	TheTimer BlasterTimer = new TheTimer();

	void Start () {
		TurretBase = TheGameObject.Self;
		BlasterTransform = BlasterPivot.GetComponent<TheTransform>();
		SelfTransform = TurretHead.GetComponent<TheTransform>();
		CannonTransform = BlasterCannon.GetComponent<TheTransform>();
		
		LBlasterTransform = LBlaster.GetComponent<TheTransform>();
		RBlasterTransform = RBlaster.GetComponent<TheTransform>();
		
		LBlasterFactory	= LBlaster.GetComponent<TheFactory>();
		LBlasterFactory.StartFactory();
		RBlasterFactory	= RBlaster.GetComponent<TheFactory>();
		RBlasterFactory.StartFactory();
		
		AudioSource = TurretBase.GetComponent<TheAudioSource>();
		
		game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");
		if(game_manager_script != null)
			TargetPlayer = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");	

		PlayerTransform = TargetPlayer.GetComponent<TheTransform>();
		
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
		float DeltaZ = TheMath.Abs(PlayerPosition.z - BlasterPosition.z);
		float DeltaY = TheMath.Abs(PlayerPosition.y - BlasterPosition.y);
		float angle = TheMath.Atan(DeltaY / DeltaZ);
		angle *= TheMath.RadToDeg;
		
		angle -= AngleOffset;
		
		if (angle < MinAngleBlasters)
			angle = MinAngleBlasters;
		else if (angle > MaxAngleBlasters)
			angle = MaxAngleBlasters;
		
		if (BlasterRotation.z >= MinAngleBlasters && BlasterRotation.z <= MaxAngleBlasters)
		{
			if (angle > BlasterRotation.z)
				BlasterTransform.SetIncrementalRotation(new TheVector3(0, 0, BlasterRotation.z + DeltaTime * BlasterRotationSpeed));
			else
				BlasterTransform.SetIncrementalRotation(new TheVector3(0, 0, BlasterRotation.z - DeltaTime * BlasterRotationSpeed));		
		}
		
		if (BlasterRotation.z < MinAngleBlasters)
			BlasterTransform.SetIncrementalRotation(new TheVector3(0, 0, MinAngleBlasters));
		else if (BlasterRotation.z > MaxAngleBlasters)
			BlasterTransform.SetIncrementalRotation(new TheVector3(0, 0, MaxAngleBlasters));

		//BlasterTransform.QuatRotation = TheQuaternion.Slerp(BlasterTransform.QuatRotation, rotation, DeltaTime * RotationSpeed);
	}
	
	void Shoot()
	{
		TheVector3 tOffset = PlayerPosition - SelfPosition;
		
		if (TheVector3.Magnitude(tOffset) < ShootingRange && TheVector3.AngleBetween(SelfTransform.ForwardDirection, tOffset) < MaxAngleBlasters*2)
		{
			if (BlasterTimer.ReadTime() >= LaserFrequency && LBlasterFactory != null && RBlasterFactory != null)
			{
				TheGameObject laser = null;
				if (shoot)
					laser = LBlasterFactory.Spawn();
				else
					laser = RBlasterFactory.Spawn();
				
				if(laser != null)
				{
					if(AudioSource != null)
						AudioSource.Play("Play_Turret_Shoot");
					
					TheScript laser_script = laser.GetScript("Laser");

					if(laser_script != null)
					{
						if (shoot)
						{
							object[] args = {TurretBase, LaserSpeed, BaseLaserDamage, LBlasterTransform.ForwardDirection, CannonTransform.QuatRotation};
							laser_script.CallFunctionArgs("SetInfo", args);
						}
						else
						{
							object[] args = {TurretBase, LaserSpeed, BaseLaserDamage, RBlasterTransform.ForwardDirection, CannonTransform.QuatRotation};
							laser_script.CallFunctionArgs("SetInfo", args);						
						}
						shoot = !shoot;
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