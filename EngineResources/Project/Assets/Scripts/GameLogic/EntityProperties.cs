using TheEngine;
using TheEngine.TheConsole; 

/*
	All ships should have this script with set with "alliance" or "empire"
	It should have a component Factory attached, with the bullet using the Laser.cs script
	Laser should have a collider and a rigid body
*/

public class EntityProperties 
{
	public string entity_faction;
	public bool is_slave1 = false;
	public bool is_ship = false;
	public bool is_turret = false;
	public bool is_generator = false;
	
	public int life = 100;
	private int modified_life = 0;
	private bool dead = false;
	private bool can_be_hit = true;

	public float laser_speed = 30;
	public int base_laser_damage = 10;

	public string explosion_prefab;
	public string tie_particles_prefab; 

	private TheGameObject game_manager = null;	
	private TheScript game_manager_script = null;
	private TheFactory factory = null;
	private TheScript movement_script = null;
	private TheTransform self_transform = null;
	private TheAudioSource audio_source = null;
	private TheScript player_movement_script = null;

    public TheGameObject slave_emmiter;
    TheAudioSource slave_audio = null;
	
	TheGameObject self = null;
	
	TheScript player_targeting_script = null;

    // Damage on Solid Collision ---
    public int damageOnSolidCollision = 0; // Put it to 100 when Raycasts are fixed

	void Init()
	{
		modified_life = life;

		self = TheGameObject.Self;

		self_transform = self.GetComponent<TheTransform>();
			
		game_manager = TheGameObject.Find("GameManager");
		
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");

		movement_script = self.GetScript("GuillemMovement");

		factory = self.GetComponent<TheFactory>();

		audio_source = self.GetComponent<TheAudioSource>();

        if (slave_emmiter != null)
        {
            slave_audio = slave_emmiter.GetComponent<TheAudioSource>();
        }

		self.tag = "Entity";

		// Add entity to game manager
		if(is_slave1)
			SetSlave1();
		
		else if(is_ship)
			SetShipFaction(entity_faction);	

		else if(is_turret)	
			SetTurret();

		else if(is_generator)
			SetGenerator();
		
		TheGameObject player_cam = TheGameObject.Find("PlayerCam");
		
		if(player_cam != null)
			player_targeting_script = player_cam.GetScript("Targeting");
	}

	void Start()
	{
		if(factory != null)
			factory.StartFactory();		
	}

	// Returns if the ship is the slave1
	bool IsSlave1()
	{
		return is_slave1;
	}

	bool IsShip()
	{
		return is_ship;
	}

	bool IsTurret()
	{
		return is_turret;
	}

	bool IsGenerator()
	{
		return is_generator;
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
					object[] args = {self, laser_speed, base_laser_damage,
									 self_transform.ForwardDirection, self_transform.QuatRotation};

					laser_script.CallFunctionArgs("SetInfo", args);
				}
			}
		}
	}
	
	// Called when the ship is hit by a laser
	void HitByShip(TheGameObject ship, int dmg)
	{
		if(ship != null && !IsDead() && can_be_hit)
		{
			TheScript entity_script = ship.GetScript("EntityProperties");

			if(entity_script != null)
			{
				string hit_faction = (string)entity_script.CallFunctionArgs("GetFaction");

				if(hit_faction == GetFaction())
				{
					// Ally hit
					DealDamage(dmg);
				}
				else
				{
					DealDamage(dmg);
				}

				// Check if is dead
				CheckDeath(ship);

				TheConsole.Log("Hit by ship");
			}
		}
	}

	void SetCanBeHit(bool set)
	{
		can_be_hit = set;
	}

	// Returns if the ship is dead or not
	bool IsDead()
	{
		return modified_life <= 0;
	}

	// Returns max life of the ship
	int GetMaxLife()
	{
		return life;
	}

	// Returns life of the ship
	int GetLife()
	{
		return modified_life;
	}

	// Returns faction of the ship
	string GetFaction()
	{
		return entity_faction;
	}
	
	// Sets life of the ship
	void SetLife(int val)
	{
		if(val != modified_life)
			TheConsole.Log("Life set to: " + val);

		modified_life = val;

		if(modified_life < 0)
			modified_life = 0;
	}

	// Deals damage to the ship
	void DealDamage(int dmg)
	{
		if(!is_slave1)
		{
			if(dmg < 0)
				dmg = 0;

			modified_life -= dmg;

			if(modified_life < 0)
				modified_life = 0;
		}
		else
		{
			if(player_movement_script != null)
			{	
				object[] args = {dmg};
                //player_movement_script.CallFunctionArgs("DamageSlaveOne", args);				
                slave_audio.Play("Play_Ship_hit");
				TheConsole.Log("DealDamage: Slave");
			}
		}
	}

	void SetSlave1()
	{
		if(game_manager_script != null)
		{
			object[] args = {self};
			game_manager_script.CallFunctionArgs("AddSlave1", args);

			player_movement_script = self.GetScript("PlayerMovement");	
		}
	}
	
	// Can be used to set ship faction dinamically
	void SetShipFaction(string faction)
	{	
		if(game_manager_script != null)
		{
			object[] args = {self};
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

	void SetTurret()
	{
		if(game_manager_script != null)
		{
			object[] args = {self};
			game_manager_script.CallFunctionArgs("AddTurret", args);
		}
	}

	void SetGenerator()
	{
		if(game_manager_script != null)
		{
			object[] args = {self};
			game_manager_script.CallFunctionArgs("AddGenerator", args);
		}
	}
	
	// Checks if ship is dead, and tell the game manager, then destroy it
	void CheckDeath(TheGameObject killer)
	{
		if(IsDead() && game_manager_script != null && !dead)
		{
			dead = true;

			if(factory != null)
				factory.ClearFactory();
			
			if(player_targeting_script != null)
			{
				TheGameObject player_target = (TheGameObject)player_targeting_script.CallFunctionArgs("GetTarget");
			
				if(player_target != null && self_transform != null)
				{
					if(self_transform == player_target.GetComponent<TheTransform>())
						player_targeting_script.CallFunctionArgs("SetToNull");
				}
			}

			if(IsShip())
			{
				if(audio_source != null){
					audio_source.Play("Play_Enemy_Explosions");
					
                }

				object[] args = {self, killer};
				game_manager_script.CallFunctionArgs("RemoveShip", args);
				
				SpawnExplosion();
				SpawnTieExplosion(); 
				
				TheGameObject.Destroy(self);					
			}

			else if(IsTurret())
			{
				object[] args = {self, killer};
				game_manager_script.CallFunctionArgs("RemoveTurret", args);
				
				SpawnExplosion();

				TheGameObject.Destroy(self);
			}

			else if(IsGenerator())
			{
				object[] args = {self, killer};
				game_manager_script.CallFunctionArgs("RemoveGenerator", args);

				SpawnExplosion();

				TheGameObject.Destroy(self);
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

	void SpawnExplosion()
	{
		TheGameObject particle = TheResources.LoadPrefab(explosion_prefab);
				
		// Particles when destroying ship
		if(particle != null)
		{
			TheTransform particle_trans = particle.GetComponent<TheTransform>();
			if(particle_trans != null && self_transform != null)
			{
				particle_trans.LocalPosition = self_transform.LocalPosition;
			
				TheScript particle_script = particle.GetScript("ParticleAutoDestroy");
				if(particle_script != null)
				{
					particle_script.CallFunctionArgs("Destroy");

					TheConsole.Log("Explosion particle created!");
				}
			}
		}
	}

	void SpawnTieExplosion()
	{
		/*TheGameObject particle = TheResources.LoadPrefab(tie_particles_prefab);
				
		// Particles when destroying ship
		if(particle != null)
		{
			TheTransform particle_trans = particle.GetComponent<TheTransform>();
			if(particle_trans != null && self_transform != null)
			{
				particle_trans.LocalPosition = self_transform.LocalPosition;
			
				TheScript particle_script = particle.GetScript("ParticleAutoDestroy");
				if(particle_script != null)
				{
					particle_script.CallFunctionArgs("Destroy");

					TheConsole.Log("Explosion particle created!");
				}
			}
		}*/
	}

    void OnCollisionEnter(TheCollisionData other)
    {
        if (other.Collider.GetGameObject() == self) return;
        if (IsShip())
        {
            DealDamage(damageOnSolidCollision);
            CheckDeath(self);
        }
        if(IsSlave1()) // Player Solid Collision
        {
			
            /*
            // Speed Filtering for Player ?? --
            // ...
            DealDamage(damageOnSolidCollision);
            CheckDeath(self);
            */
        }
    }
}