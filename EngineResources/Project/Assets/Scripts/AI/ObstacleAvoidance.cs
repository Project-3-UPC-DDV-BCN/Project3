using TheEngine;

public class ObstacleAvoidance {

	public float rayLength = 50f;
	public float avoidingForce = 40f;
	float avoidingMultiplier = 1f;

	// Transforms
	TheTransform transform = null;

	// RayCasts Directions
	public TheVector3 rayCenter = new TheVector3(0, 0, 1);
	public TheVector3 rayLeft = new TheVector3(-1, 0, 1);
	public TheVector3 rayRight = new TheVector3(1, 0, 1);
	public TheVector3 rayTop = new TheVector3(0, 1, 1);
	public TheVector3 rayBottom = new TheVector3(0, -1, 1);

	void Start () {
		transform = TheGameObject.Self.GetComponent<TheTransform>();
	}
	
	void Update () {
		if(transform == null) return;
		
		// RayCast Setup ---
		TheVector3 RayCenter = rayCenter + transform.ForwardDirection;
		TheVector3 RayLeft = rayLeft + transform.ForwardDirection;
		TheVector3 RayRight = rayRight + transform.ForwardDirection;
		TheVector3 RayTop = rayTop + transform.ForwardDirection;
		TheVector3 RayBottom = rayBottom + transform.ForwardDirection;

		// RayCasting ---
		TheRayCastHit rayHitCenter = ThePhysics.RayCast(transform.GlobalPosition, RayCenter, rayLength);
		TheRayCastHit rayHitLeft = ThePhysics.RayCast(transform.GlobalPosition, RayLeft, rayLength);
		TheRayCastHit rayHitRight = ThePhysics.RayCast(transform.GlobalPosition, RayRight, rayLength);
		TheRayCastHit rayHitTop = ThePhysics.RayCast(transform.GlobalPosition, RayTop, rayLength);
		TheRayCastHit rayHitBottom = ThePhysics.RayCast(transform.GlobalPosition, RayBottom, rayLength);
		
		// RayHit Managing ---
		TheVector3 avoidanceVector = new TheVector3(0, 0, 0);
		// Center
		if(rayHitCenter != null) {
			avoidanceVector = new TheVector3(
				avoidanceVector.x + rayHitCenter.Normal.x, 
				avoidanceVector.y + rayHitCenter.Normal.y, 
				avoidanceVector.z + rayHitCenter.Normal.z
			);
		}
		// Left
		if(rayHitLeft != null) {
			avoidanceVector = new TheVector3(
				avoidanceVector.x + rayHitLeft.Normal.x, 
				avoidanceVector.y + rayHitLeft.Normal.y, 
				avoidanceVector.z + rayHitLeft.Normal.z
			);
		}
		// Right
		if(rayHitRight != null) {
			avoidanceVector = new TheVector3(
				avoidanceVector.x + rayHitRight.Normal.x, 
				avoidanceVector.y + rayHitRight.Normal.y, 
				avoidanceVector.z + rayHitRight.Normal.z
			);
		}
		// Top
		if(rayHitTop != null) {
			avoidanceVector = new TheVector3(
				avoidanceVector.x + rayHitTop.Normal.x, 
				avoidanceVector.y + rayHitTop.Normal.y, 
				avoidanceVector.z + rayHitTop.Normal.z
			);
		}
		// Bottom
		if(rayHitBottom != null) {
			avoidanceVector = new TheVector3(
				avoidanceVector.x + rayHitBottom.Normal.x, 
				avoidanceVector.y + rayHitBottom.Normal.y, 
				avoidanceVector.z + rayHitBottom.Normal.z
			);
		}
		
		// Rotation Managing ---
		TheVector3 finalRotation = avoidanceVector.Normalized * avoidingForce * avoidingMultiplier;
		
		transform.LocalRotation = new TheVector3(
			transform.LocalRotation.x + finalRotation.y,
			transform.LocalRotation.y + finalRotation.x,
			transform.LocalRotation.z
		);

	}
}