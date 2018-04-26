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
		
		// RayCasting ---
		TheRayCastHit rayHitCenter = ThePhysics.RayCast(transform.GlobalPosition, rayCenter, rayLength);
		
		// RayHit Managing ---
		TheVector3 avoidanceVector = new TheVector3(0, 0, 0);
		// Center
		avoidanceVector = new TheVector3(
			avoidanceVector.x + rayHitCenter.Normal.x, 
			avoidanceVector.y + rayHitCenter.Normal.y, 
			avoidanceVector.z + rayHitCenter.Normal.z
		);
		
		// Left
		// Right
		// Top
		// Bottom
		
		// Rotation Managing ---
		TheVector3 finalRotation = avoidanceVector.Normalized * avoidingForce * avoidingMultiplier;
		
		transform.LocalRotation = new TheVector3(
			transform.LocalRotation.x + finalRotation.x,
			transform.LocalRotation.y + finalRotation.y,
			transform.LocalRotation.z
		);

	}
}