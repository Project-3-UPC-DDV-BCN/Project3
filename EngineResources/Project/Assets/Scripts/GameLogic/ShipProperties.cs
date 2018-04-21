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
	private TheScript movement_script = null;
	private TheTransform self_transform = null;
	private TheAudioSource audio_source = null;
	private TheScript player_movement_script = null;

	bool one_shoot = true;
		
	void Init()
	{
		TheGameObject.Self.tag = "ShipEntity";

		self_transform = TheGameObject.Self.GetComponent<TheTransform>();
			
		game_manager = TheGameObject.Find("GameManager");
		
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");

		movement_script = TheGameObject.Self.GetScript("GuillemMovement");

		factory = TheGameObject.Self.GetComponent<TheFactory>();

		audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();

		// Add ship to game manager
		if(is_slave1)
		{
			if(game_manager_script != null)
			{
				object[] args = {TheGameObject.Self};
				game_manager_script.CallFunctionArgs("AddSlave1", args);

				player_movement_script = TheGameObject.Self.GetScript("PlayerMovement");
			}
		}
		else
			SetShipFaction(ship_faction);			
	}

	void Start()
	{
		if(factory != null)
			factory.StartFactory();		
	}

	void Update()
	{
		// Temp ----------------------
		/*if (TheInput.IsKeyDown("w"))
		{
			if(one_shoot)
				Shoot();

			one_shoot = false;
		}
		else
			one_shoot = true;
		*/
		// ---------------------------
		
		CheckDeath();
	}

	// Returns if the ship is the slave1
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
				if(audio_source != null)
					audio_source.Play("Play_Shoot");
				
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
					if(!is_slave1)
						TheConsole.Log("Ally hit. Dmg: " + dmg + "  | Ship now has: " + GetLife() + " Life");
					else
						TheConsole.Log("Slave1 is hit!");
				}
				else
				{
					// Enemy hit
					DealDamage(dmg);
					if(!is_slave1)
						TheConsole.Log("Enemy hit. Dmg: " + dmg+ "  | Ship now has: " + GetLife() + " Life");
					else
						TheConsole.Log("Slave1 is hit!");
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
	
	// Sets life of the ship
	void SetLife(int set)
	{
		life = set;

		if(life < 0)
			life = 0;
	}

	// Deals damage to the ship
	void DealDamage(int dmg)
	{
		if(!is_slave1)
		{
			if(dmg < 0)
				dmg = 0;

			life -= dmg;

			if(life < 0)
				life = 0;
		}
		else
		{
			if(player_movement_script != null)
			{
				object[] args = {(float)dmg};
				player_movement_script.CallFunctionArgs("DamageSlaveOne", args);
			}
		}
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
	
	// Checks if ship is dead, and tell the game manager, then destroy it
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
				if(audio_source != null)
					audio_source.Play("Play_Enemy_Explosions");

				TheGameObject particle = TheResources.LoadPrefab("ParticleExplosion");
				
				// Particles when destroying ship
				if(particle != null)
				{
					TheTransform particle_trans = particle.GetComponent<TheTransform>();
					if(particle_trans != null && self_transform != null)
					{
						TheConsole.Log("Explosion particle created!");
						particle_trans.GlobalPosition = self_transform.GlobalPosition;
					}
				}

				object[] args = {TheGameObject.Self};
				game_manager_script.CallFunctionArgs("RemoveShip", args);
				
				TheGameObject.Destroy(TheGameObject.Self);					
			}
		}
	}

	// Function called by the game manager when a ship of the scene is killed
	// We check if the destroyed ship is our target, and if it is, we change target
	void OnShipDestroyedCallback(TheGameObject ship)
	{
		if(movement_script != null)
		{
			object[] args = {ship};
			movement_script.CallFunctionArgs("ClearIfTarget", args);
		}
	}
}