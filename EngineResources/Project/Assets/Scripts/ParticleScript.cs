using TheEngine;
//using TheEngine.TheConsole; 

public class ParticleScript {

	TheParticleEmmiter test; 
	private float time; 
	private bool done;

	void Start () {
		test = TheGameObject.Self.GetComponent<TheParticleEmmiter>();
		test.Play();
		done = false; 
		time = 0; 
		test.SetEmitterSpeed(15.0f); 
	}
	
	void Update ()
   {
		time++; 
		//TheConsole.Log(time.ToString()); 
		if(time > 500 && done == false)
		{	
			test.SetParticleSpeed(70.0f);
			done = true; 
		}
	}
}