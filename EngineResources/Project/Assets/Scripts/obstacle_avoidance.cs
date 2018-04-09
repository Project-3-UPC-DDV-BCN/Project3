using TheEngine;
using TheEngine.TheConsole;
using TheEngine.TheDebug;

public class obstacle_avoidance {
	
	private TheRay[] rays = new TheRay[1];
	
	private TheTransform transform;
	private TheTransform target_transform;
	
	public float speed = 30;
	public int direction = 50;
	
	public TheGameObject target;

	void Start () {
		transform = TheGameObject.Self.GetComponent<TheTransform>();
		/*if(target == null)
			TheConsole.Log("null!");*/
		target_transform = target.GetComponent<TheTransform>();
	}
	
	void Update () {
		
		/*if(target_transform == null)
			TheConsole.Log("Hey!");
		TheVector3 targetDir = target_transform.GlobalPosition - transform.GlobalPosition;
		
		transform.QuatRotation = TheQuaternion.Slerp(transform.QuatRotation, TheVector3.LookRotation(targetDir, TheVector3.Up),speed * TheTime.DeltaTime);*/
		
		//rays[0] = new TheRay(transform.GlobalPosition, transform.ForwardDirection + (transform.RightDirection * 0.1f));
		//rays[1] = new TheRay(transform.GlobalPosition, transform.GlobalPosition + (transform.RightDirection * -0.1f));
		
		//TheVector3 target_pos = transform.GlobalPosition;
		
		//bool rotate_to_avoid = false;
		
		//foreach(TheRay ray in rays)
		//{
			//TheRayCastHit hit = ThePhysics.RayCast(ray, 50);
			//if(hit != null)
			//{
			//	int direction = 40;
				/*if(ray == rays[1])
				{
					direction *= -1;
				}*/
			//	TheVector3 target_pos = hit.ContactPoint + hit.Normal * direction;
				
			//	TheVector3 targetDir = target_pos - transform.GlobalPosition;
		
			//	transform.QuatRotation = TheQuaternion.Slerp(transform.QuatRotation, TheVector3.LookRotation(targetDir, TheVector3.Up),speed * TheTime.DeltaTime);
			//}
		//}
		
		TheVector3 targetDir = (target_transform.GlobalPosition - transform.GlobalPosition).Normalized;
		TheRay mid = new TheRay(transform.GlobalPosition, transform.ForwardDirection);
		TheRayCastHit hit = ThePhysics.RayCast(mid, 50);
		
		if(hit != null)
		{
			targetDir += hit.Normal * direction;
		}
		
		TheRay left = new TheRay(transform.GlobalPosition, transform.ForwardDirection + (new TheVector3(0,0,0.1f) * transform.RightDirection));
		TheRay right = new TheRay(transform.GlobalPosition, transform.ForwardDirection + (new TheVector3(0,0,-0.1f) * transform.RightDirection));
		
		TheConsole.Log(left + " " + right);
		
		TheDebugDraw.Ray(mid.Origin, mid.Direction * 50, TheColor.Green);
		TheDebugDraw.Ray(left.Origin, left.Direction * 50, TheColor.Red);
		TheDebugDraw.Ray(right.Origin, right.Direction * 50, TheColor.Red);
		
		TheRayCastHit hit2 = ThePhysics.RayCast(left, 50);
		
		if(hit2 != null)
		{
			targetDir += hit2.Normal * direction;
		}
		
		TheRayCastHit hit3 = ThePhysics.RayCast(right, 50);
			
		if(hit3 != null)
		{
			targetDir += hit3.Normal * direction;
		}
		
		TheQuaternion rot = TheVector3.LookRotation(targetDir, TheVector3.Up);
		
		transform.QuatRotation = TheQuaternion.Slerp(transform.QuatRotation, rot, TheTime.DeltaTime);
		
		transform.GlobalPosition += transform.ForwardDirection * speed * TheTime.DeltaTime;
	}
}