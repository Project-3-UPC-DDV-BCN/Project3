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
	public bool is_slave1 = false;

	public float laser_speed = 30;
	public int base_laser_damage = 10;

	private int life = 100;
	private bool dead = false;

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

		// Add ship to game manager
		if(is_slave1)
		{
			if(game_manager_script != null)
			{
				object[] args = {TheGameObject.Self};
				game_manager_script.CallFunctionArgs("AddSlave1", args);
			}
		}
		else
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
		// Temp ----------------------
		if (TheInput.IsKeyDown("w"))
		{
			if(one_shoot)
				Shoot();

			one_shoot = false;
		}
		else
			one_shoot = true;
		// ---------------------------
		
		CheckDeath();
	}

	bool IsSlave1()
	{
		return is_slave1;
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
					object[] args = {TheGameObject.Self, laser_speed, base_laser_damage,
									 self_transform.ForwardDirection, self_transform.QuatRotation};

					laser_script.CallFunctionArgs("SetInfo", args);
				}
			}
		}
	}
	
	// Called when the ship is hit by a laser
	void HitByShip(TheGameObject ship, int dmg)
	{
		if(ship != null && !IsDead())
		{
			TheScript ship_script = ship.GetScript("ShipProperties");
			if(ship_script != null)
			{
				string hit_faction = (string)ship_script.CallFunctionArgs("GetFaction");

				if(hit_faction == GetFaction())
				{
					// Ally hit
					DealDamage(dmg);
					TheConsole.Log("Ally hit. Dmg: " + dmg + "  | Ship now has: " + GetLife() + " Life");
				}
				else
				{
					// Enemy hit
					DealDamage(dmg);
					TheConsole.Log("Enemy hit. Dmg: " + dmg+ "  | Ship now has: " + GetLife() + " Life");
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
	
	void SetLife(int set)
	{
		life = set;

		if(life < 0)
			life = 0;
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

	void CheckDeath()
	{
		if(IsDead() && game_manager_script != null && !dead)
		{
			dead = true;

			if(factory != null)
				factory.ClearFactory();
			
			if(IsSlave1())
			{
				game_manager_script.CallFunctionArgs("Lose");
			}
			else
			{
				object[] args = {TheGameObject.Self};
				game_manager_script.CallFunctionArgs("RemoveShip", args);
				TheGameObject.Destroy(TheGameObject.Self);					
			}
		}
	}
}