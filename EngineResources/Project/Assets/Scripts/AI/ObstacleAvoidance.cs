using TheEngine;
using TheEngine.TheDebug;
using TheEngine.TheMath;

public class ObstacleAvoidance {

	public float rayLength = 50f;
	public float avoidingForce = 40f;
	public float avoidingMultiplier = 1f;

	// Transforms
	TheTransform transform = null;

	// RayCasts Directions
	public TheVector3 rayCenter = new TheVector3(0, 0, 1);
	public TheVector3 rayLeft = new TheVector3(-1, 0, 1);
	public TheVector3 rayRight = new TheVector3(1, 0, 1);
	public TheVector3 rayTop = new TheVector3(0, 1, 1);
	public TheVector3 rayBottom = new TheVector3(0, -1, 1);

	// Exceptions
	public TheBoxCollider self_collider = null;

	void Start () {
		transform = TheGameObject.Self.GetComponent<TheTransform>();
		if(self_collider == null) self_collider = TheGameObject.Self.GetComponent<TheBoxCollider>();
	}
	
	void Update () {
		if(transform == null) return;

		// RayCast Setup ---
		TheVector3 RayCenter = transform.ForwardDirection;
		TheVector3 RayLeft = rayLeft + transform.ForwardDirection;
		TheVector3 RayRight = rayRight + transform.ForwardDirection;
		TheVector3 RayTop = rayTop + transform.ForwardDirection;
		TheVector3 RayBottom = rayBottom + transform.ForwardDirection;

		// RayDebug ---
		/*TheDebugDraw.Ray(transform.GlobalPosition, RayCenter.Normalized * rayLength, TheColor.Red);
		TheDebugDraw.Ray(transform.GlobalPosition, RayLeft.Normalized * rayLength, TheColor.Blue);	
		TheDebugDraw.Ray(transform.GlobalPosition, RayRight.Normalized * rayLength, TheColor.Blue);	
		TheDebugDraw.Ray(transform.GlobalPosition, RayTop.Normalized * rayLength, TheColor.Blue);	
		TheDebugDraw.Ray(transform.GlobalPosition, RayBottom.Normalized * rayLength, TheColor.Blue);*/			

		// RayCasting ---
		TheRayCastHit[] ArrRayHitCenter = ThePhysics.RayCastAll(transform.GlobalPosition, RayCenter, rayLength);
		TheRayCastHit[] ArrRayHitLeft = ThePhysics.RayCastAll(transform.GlobalPosition, RayLeft, rayLength);
		TheRayCastHit[] ArrRayHitRight = ThePhysics.RayCastAll(transform.GlobalPosition, RayRight, rayLength);
		TheRayCastHit[] ArrRayHitTop = ThePhysics.RayCastAll(transform.GlobalPosition, RayTop, rayLength);
		TheRayCastHit[] ArrRayHitBottom = ThePhysics.RayCastAll(transform.GlobalPosition, RayBottom, rayLength);

		// RayCast Filtering ---
		TheRayCastHit rayHitCenter = null;
		TheRayCastHit rayHitLeft = null;
		TheRayCastHit rayHitRight = null;
		TheRayCastHit rayHitTop = null;
		TheRayCastHit rayHitBottom = null;
		// Center
		if(ArrRayHitCenter.Length > 0) {
			foreach(TheRayCastHit hit in ArrRayHitCenter) {
				if(hit.Collider != self_collider) {
					rayHitCenter = hit;
					break;
				}
			}
		}
		// Left
		if(ArrRayHitLeft.Length > 0) {
			foreach(TheRayCastHit hit in ArrRayHitLeft) {
				if(hit.Collider != self_collider) {
					rayHitLeft = hit;
					break;
				}
			}
		}	
		// Right
		if(ArrRayHitRight.Length > 0) {
			foreach(TheRayCastHit hit in ArrRayHitRight) {
				if(hit.Collider != self_collider) {
					rayHitRight = hit;
					break;
				}
			}
		}
		// Top
		if(ArrRayHitTop.Length > 0) {
			foreach(TheRayCastHit hit in ArrRayHitTop) {
				if(hit.Collider != self_collider) {
					rayHitTop = hit;
					break;
				}
			}
		}
		// Bottom	
		if(ArrRayHitBottom.Length > 0) {
			foreach(TheRayCastHit hit in ArrRayHitBottom) {
				if(hit.Collider != self_collider) {
					rayHitBottom = hit;
					break;
				}
			}
		}

		// RayHit Managing ---
		TheVector3 avoidanceVector = new TheVector3(0, 0, 0);
		// Center
		if(rayHitCenter != null) {
			avoidanceVector = new TheVector3(
				avoidanceVector.x + rayHitCenter.Normal.x, 
				avoidanceVector.y + rayHitCenter.Normal.y, 
				avoidanceVector.z + rayHitCenter.Normal.z
			);
			if(rayHitCenter.Normal == TheVector3.Forward || rayHitCenter.Normal == TheVector3.BackWard)
				avoidanceVector.y = avoidanceVector.y + 1f;
			avoidingMultiplier = 1 - (rayHitCenter.Distance / rayLength);
		}
		// Left
		if(rayHitLeft != null) {
			avoidanceVector = new TheVector3(
				avoidanceVector.x + rayHitLeft.Normal.x, 
				avoidanceVector.y + rayHitLeft.Normal.y, 
				avoidanceVector.z + rayHitLeft.Normal.z
			);
			if(rayHitLeft.Normal == TheVector3.Forward || rayHitLeft.Normal == TheVector3.BackWard)
				avoidanceVector.x = avoidanceVector.x + 1f;
			avoidingMultiplier = 1 - (rayHitLeft.Distance / rayLength);
		}
		// Right
		if(rayHitRight != null) {
			avoidanceVector = new TheVector3(
				avoidanceVector.x + rayHitRight.Normal.x, 
				avoidanceVector.y + rayHitRight.Normal.y, 
				avoidanceVector.z + rayHitRight.Normal.z
			);
			if(rayHitRight.Normal == TheVector3.Forward || rayHitRight.Normal == TheVector3.BackWard)
				avoidanceVector.x = avoidanceVector.x - 1f;
			avoidingMultiplier = 1 - (rayHitRight.Distance / rayLength);
		}
		// Top
		if(rayHitTop != null) {
			avoidanceVector = new TheVector3(
				avoidanceVector.x + rayHitTop.Normal.x, 
				avoidanceVector.y + rayHitTop.Normal.y, 
				avoidanceVector.z + rayHitTop.Normal.z
			);
			if(rayHitTop.Normal == TheVector3.Forward || rayHitTop.Normal == TheVector3.BackWard)
				avoidanceVector.y = avoidanceVector.y - 1f;
			avoidingMultiplier = 1 - (rayHitTop.Distance / rayLength);
		}
		// Bottom
		if(rayHitBottom != null) {
			avoidanceVector = new TheVector3(
				avoidanceVector.x + rayHitBottom.Normal.x, 
				avoidanceVector.y + rayHitBottom.Normal.y, 
				avoidanceVector.z + rayHitBottom.Normal.z
			);
			if(rayHitBottom.Normal == TheVector3.Forward || rayHitBottom.Normal == TheVector3.BackWard)
				avoidanceVector.y = avoidanceVector.y + 1f;
			avoidingMultiplier = 1 - (rayHitBottom.Distance / rayLength);
		}
		
		// Rotation Managing ---
		TheVector3 finalRotation = avoidanceVector.Normalized * avoidingForce * avoidingMultiplier;
		
		transform.LocalRotation = new TheVector3(
			transform.LocalRotation.x + finalRotation.y,
			transform.LocalRotation.y + finalRotation.x,
			transform.LocalRotation.z
		);

	}

	TheVector3 RotateVector(TheVector3 v, TheVector3 euler) {
		TheVector3 ret = v;
		// Conversion to Rad ---
		euler = new TheVector3(TheMath.DegToRad * euler.x, TheMath.DegToRad * euler.y, TheMath.DegToRad * euler.z);
		// Rotations ---
		// Z - Axis (Necessary?)
		// ...
		// Y - Axis
		ret.x = ret.x * TheMath.Cos(euler.y) + ret.z * TheMath.Sin(euler.y);
		ret.z = -ret.x * TheMath.Sin(euler.y) + ret.z * TheMath.Cos(euler.y);
		// X - Axis
		ret.y = ret.y * TheMath.Cos(euler.x) - ret.z * TheMath.Sin(euler.x);
		ret.z = ret.y * TheMath.Sin(euler.x) + ret.z * TheMath.Cos(euler.x);
		// Conversion to Deg ---
		//ret = new TheVector3(TheMath.RadToDeg * ret.x, TheMath.RadToDeg * ret.y, TheMath.RadToDeg * ret.z);

		return ret;
	}

}