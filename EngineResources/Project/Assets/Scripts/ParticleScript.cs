using TheEngine;

public class ParticleScript {

	TheParticleEmmiter test; 

	void Start () {
		test = TheGameObject.Self.GetComponent<TheParticleEmmiter>();
		test.Play();
	}
	
	void Update () {
		
	}
}