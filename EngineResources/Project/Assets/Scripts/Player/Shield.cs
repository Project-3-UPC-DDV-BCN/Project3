using TheEngine;
using TheEngine.TheConsole;

public class Shield {
	
	public TheGameObject Slave;
	private TheScript slave_script = null;
	
	public bool front_shield = false;
	public bool back_shield = false;

	void Start () 
	{
		if(Slave != null)
		{
			slave_script = Slave.GetScript("PlayerMovement");
		}
	}
	
	void Update () 
	{
		
	}
	
	void OnTriggerEnter(TheCollisionData coll)
	{

		if(coll == null)
			return;

		TheGameObject colision_object = coll.Collider.GetGameObject();

		if(colision_object == null)
			return;
		
		TheScript laser = colision_object.GetScript("Laser");
		

		if(laser != null && slave_script != null)
		{
			int dmg = (int)laser.CallFunctionArgs("GetDamage");
			object[] args = {dmg};
			colision_object.SetActive(false);
			
			if(front_shield && back_shield)
				TheConsole.Log("Invalid Shield Options");
			else if(front_shield)
				slave_script.CallFunctionArgs("DamageFrontShield", args);
			else if(back_shield)
				slave_script.CallFunctionArgs("DamageBackShield", args);
			else
				TheConsole.Log("Invalid Shield Options");
				
		}
	}
}