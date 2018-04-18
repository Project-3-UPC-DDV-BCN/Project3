using TheEngine;
using TheEngine.TheConsole;

/*
	All ships should have this script with set with "alliance" or "empire"
	It should have a component Factory attached, with the bullet using the Laser.cs script
	Laser should have a collider and a rigid body
*/

public class ShipProperties 
{
	public string ship_faction;
	public float laser_speed = 30;

	private int life = 100;

	private TheGameObject game_manager = null;	
	private TheScript game_manager_script = null;
	private TheFactory factory = null;

	private TheTransform self_transform = null;

	bool one_shoot = true;
		
	void Init()
	{
		TheGameObject.Self.tag = "ShipEntity";

		self_transform = TheGameObject.Self.GetComponent<TheTransform>();
			
		game_manager = TheGameObject.Find("GameManager");
		
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");

		SetShipFaction(ship_faction);			
	}

	void Start()
	{
		factory = TheGameObject.Self.GetComponent<TheFactory>();

		if(factory != null)
			factory.StartFactory();
	}

	void Update()
	{
		if (TheInput.GetControllerJoystickMove(0, "LEFT_TRIGGER") >= 20000)
		{
			if(one_shoot)
				Shoot();

			one_shoot = false;
		}
		else
			one_shoot = true;
	}
	
	// Shoots a laser
	void Shoot()
	{
		if(factory != null)
		{
			TheGameObject laser = factory.Spawn();
			
			if(laser != null)
			{
				TheScript laser_script = laser.GetScript("Laser");

				if(laser_script != null)
				{
					object[] args = {TheGameObject.Self, laser_speed, 
									 self_transform.ForwardDirection, self_transform.QuatRotation};

					laser_script.CallFunctionArgs("SetInfo", args);
				}
			}
		}
	}
	
	// Called when the ship is hit by a laser
	void HitByShip(TheGameObject ship)
	{
		if(ship != null && ship != TheGameObject.Self)
		{
			TheScript ship_script = ship.GetScript("ShipProperties");
			if(ship_script != null)
			{
				string hit_faction = (string)ship_script.CallFunctionArgs("GetFaction");

				if(hit_faction == GetFaction())
				{
					TheConsole.Log("Ally hit");
					// Ally hit
				}
				else
				{
					TheConsole.Log("Enemy hit");
					// Enemy hit
				}
			}
		}
	}

	// Returns if the ship is dead or not
	bool IsDead()
	{
		return life <= 0;
	}

	// Returns life of the ship
	int GetLife()
	{
		return life;
	}

	// Returns faction of the ship
	string GetFaction()
	{
		return ship_faction;
	}
	
	// Deals damage to the ship
	void DealDamage(int dmg)
	{
		if(dmg < 0)
			dmg = 0;

		life -= dmg;

		if(life < 0)
			life = 0;
	}
	
	// Can be used to set ship faction dinamically
	void SetShipFaction(string faction)
	{	
		if(game_manager_script != null)
		{
			object[] args = {TheGameObject.Self};
			game_manager_script.CallFunctionArgs("RemoveShip", args);
			
			if(faction == "alliance")
			{
				game_manager_script.CallFunctionArgs("AddAllianceShip", args);
			}
			else if (faction == "empire")
			{
				game_manager_script.CallFunctionArgs("AddEmpireShip", args);
			}

			
		}
	}
}