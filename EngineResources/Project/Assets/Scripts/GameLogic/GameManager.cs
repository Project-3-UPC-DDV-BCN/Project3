using TheEngine;
using System.Collections.Generic; 
using TheEngine.TheConsole; 

public class GameManager
{
	private string team = "no_str"; 

	TheScript training_mode_script = null;
	TheScript level1_script = null;
	TheScript markers_script = null; 

	public TheGameObject front_radar_go;
	TheRadar front_radar = null;
	public TheGameObject back_radar_go;
	TheRadar back_radar = null;

	List<TheGameObject> alliance_ships = new List<TheGameObject>();
    List<TheGameObject> empire_ships = new List<TheGameObject>();
	List<TheGameObject> turret_entities = new List<TheGameObject>();
	List<TheGameObject> generator_entities = new List<TheGameObject>();
	TheGameObject slave1 = null;
	TheTransform slave1_trans = null;
	
	string alliance_name = "alliance";
	string empire_name = "empire";

	public bool is_training_mode = false;
	public bool is_level1 = false;
	
	float last_time_scale = 1;
	
	public TheGameObject controller_image;
	public TheGameObject cockpit_image;
	bool game_paused = false;
	bool is_controller_image;

	void Init ()
	{
		team = TheData.GetString("faction");

		if(team == "no_str")
			team = "alliance";

        TheConsole.Log("Team: " + team); 

		if(is_training_mode)
		{
			training_mode_script = TheGameObject.Self.GetScript("TrainingModeManager");
			TheConsole.Log("Training mode enabled!"); 
		}

		if(is_level1)
		{
			level1_script = TheGameObject.Self.GetScript("Level1Manager");
			TheConsole.Log("Level 1 enabled!");
		}

		// Raders
		if(front_radar_go != null)
			front_radar = front_radar_go.GetComponent<TheRadar>();
	
		if(back_radar_go != null)
			back_radar = back_radar_go.GetComponent<TheRadar>();

		markers_script = TheGameObject.Self.GetScript("EnemyMarker"); 
	}

	void Start ()
	{
		if(slave1 != null)
			slave1_trans = slave1.GetComponent<TheTransform>();
		
		if(controller_image != null)
			PauseGame(true);
	}
	
	void Update()
	{
		if(TheInput.GetControllerButton(0, "CONTROLLER_START") == 1 || TheInput.IsKeyDown("ESC"))
		{
			PauseGame(!game_paused);
		}
		
		if(game_paused)
		{
			if(is_controller_image)
			{
				if(TheInput.GetControllerButton(0, "CONTROLLER_RIGHT_ARROW") == 1 || TheInput.IsKeyDown("RIGHT_ARROW"))
				{
					cockpit_image.SetActive(true);
					controller_image.SetActive(false);
					is_controller_image = false;
				}
			}
			else
			{
				if(TheInput.GetControllerButton(0, "CONTROLLER_LEFT_ARROW") == 1 || TheInput.IsKeyDown("LEFT_ARROW"))
				{
					controller_image.SetActive(true);
					cockpit_image.SetActive(false);
					is_controller_image = true;
				}
			}
		}
	}
	
	void PauseGame(bool pause)
	{
		if(pause)
		{
			last_time_scale = TheTime.TimeScale;
			TheTime.TimeScale = 0;
		}
		else
		{
			TheTime.TimeScale = last_time_scale;
		}
		
		if(controller_image != null)
		{
			controller_image.SetActive(pause);
			is_controller_image = true;
		}
		
		if(cockpit_image != null)
		{
			cockpit_image.SetActive(false);
		}

		game_paused = pause;
		
		TheApplication.DrawMouse(pause);
	}

	bool GetIsTrainingMode()
	{
		return is_training_mode;
	}

	bool GetIsLevel1()
	{
		return is_level1;
	}

	void AddSlaveEnemiesShip(TheGameObject go)
	{
		string faction = team;

		if(team == alliance_name)
		{
			AddEmpireShip(go);
		}
		else if(team == empire_name)
		{
			AddAllianceShip(go);
		}
	}

	void AddSlave1(TheGameObject slave)
	{
		slave1 = slave;

		if(slave != null)
		{
			TheScript slave1_properties = slave.GetScript("EntityProperties");
	
			if(slave1_properties != null)
			{
				bool is_slave = (bool)slave1_properties.CallFunctionArgs("IsSlave1");		
		
				if(is_slave)
				{
					if(team == alliance_name || team == empire_name)
					{
						object[] args = {team};
						slave1_properties.CallFunctionArgs("SetShipFaction", args);
				
						slave1 = slave;

						TheConsole.Log("Slave1 added to " + team);
					}
				}
			}
		}
	}

	void AddAllianceShip(TheGameObject add)
	{
		if(add != null)
		{
			TheScript entity_properties = add.GetScript("EntityProperties");

			if(entity_properties != null)
			{	
				bool is_slave = (bool)entity_properties.CallFunctionArgs("IsSlave1");		
				if(!is_slave)
				{
					alliance_ships.Add(add);

					AddToRadar(add, "Alliance");
					
					TheConsole.Log("Ship added to alliance!: " + AllianceShipsCount());
				}
			}
		}
	}

	void AddEmpireShip(TheGameObject add)
	{
		if(add != null)
		{
			TheScript entity_properties = add.GetScript("EntityProperties");
			
			if(entity_properties != null)
			{
				bool is_slave = (bool)entity_properties.CallFunctionArgs("IsSlave1");		
				if(!is_slave)
				{
					empire_ships.Add(add);
					markers_script.CallFunction("UpdateShipList");
				
					AddToRadar(add, "Empire");

					TheConsole.Log("Ship added to empire!: " + EmpireShipsCount());
				}
			}
		}
	}

	void AddTurret(TheGameObject add)
	{
		if(add != null)
		{
			TheScript ship_prop = add.GetScript("EntityProperties");
			
			if(ship_prop != null)
			{
				turret_entities.Add(add);
				TheConsole.Log("Turret added: " + TurretsCount());
			}
		}
	}

	void AddGenerator(TheGameObject add)
	{
		if(add != null)
		{
			TheScript ship_prop = add.GetScript("EntityProperties");
			
			if(ship_prop != null)
			{
				generator_entities.Add(add);
				TheConsole.Log("Generator added: " + TurretsCount());
			}
		}
	}	

	void RemoveShip(TheGameObject remove, TheGameObject killer)
	{
		bool found = false;
		if(empire_ships.Remove(remove))
		{
			found = true;
			TheConsole.Log("Ship destroyed from empire! Remaining: " + EmpireShipsCount());
		}

		if(alliance_ships.Remove(remove))
		{
			found = true;
			TheConsole.Log("Ship destroyed from alliance! Remaining: " + AllianceShipsCount());
		}		

		if(found)
		{
			for(int i = 0; i < alliance_ships.Count; ++i)
			{
				TheScript ship_prop = alliance_ships[i].GetScript("EntityProperties");
				if(ship_prop != null)
				{
					object[] args = {remove};
					ship_prop.CallFunctionArgs("OnShipDestroyedCallback", args);
				}
			}

			for(int i = 0; i < empire_ships.Count; ++i)
			{
				TheScript ship_prop = empire_ships[i].GetScript("EntityProperties");
				if(ship_prop != null)
				{
					object[] args = {remove};
					ship_prop.CallFunctionArgs("OnShipDestroyedCallback", args);
				}
			}
			
			if(front_radar != null)
				front_radar.RemoveEntity(remove);
	
			if(back_radar != null)
				back_radar.RemoveEntity(remove);

			if(is_training_mode)
			{
				if(training_mode_script != null)
				{
					object[] args = {remove, killer};
					training_mode_script.CallFunctionArgs("OnShipDestroyedCallback", args);
				}
			}
			
			if(is_level1)
			{
				if(level1_script != null)
				{
					object[] args = {remove, killer};
					level1_script.CallFunctionArgs("OnShipDestroyedCallback", args);
				}
			}
		}
	}

	void RemoveTurret(TheGameObject turret, TheGameObject killer)
	{
		if(turret != null)
		{
			if(turret_entities.Remove(turret))
			{
				if(front_radar != null)
					front_radar.RemoveEntity(turret);
	
				if(back_radar != null)
					back_radar.RemoveEntity(turret);
				
				if(is_level1)
				{
					if(level1_script != null)
					{
						object[] args = {turret, killer};
						level1_script.CallFunctionArgs("OnTurretDestroyedCallback", args);
					}
				}

				TheConsole.Log("Turret destroyed! Remaining: " + TurretsCount());
			}
		}
	}

	void RemoveGenerator(TheGameObject generator, TheGameObject killer)
	{
		if(generator != null)
		{
			if(generator_entities.Remove(generator))
			{
				if(front_radar != null)
					front_radar.RemoveEntity(generator);
	
				if(back_radar != null)
					back_radar.RemoveEntity(generator);
				
				if(is_level1)
				{
					if(level1_script != null)
					{
						object[] args = {generator, killer};
						level1_script.CallFunctionArgs("OnGeneratorDestroyedCallback", args);
					}
				}

				TheConsole.Log("Generator destroyed! Remaining: " + TurretsCount());
			}
		}
	}

	List<TheGameObject> GetSlaveEnemies()
	{
		List<TheGameObject> ret = new List<TheGameObject>();
		
		string faction = team;

		if(team == alliance_name)
		{
			ret = empire_ships;
		}
		else if(team == empire_name)
		{
			ret = alliance_ships;
		}
			
		return ret;
	}

	int GetSlaveEnemiesCount()
	{
		int ret = 0;

		string faction = team;

		if(team == alliance_name)
		{
			ret = empire_ships.Count;
		}
		else if(team == empire_name)
		{
			ret = alliance_ships.Count;
		}

		return ret;
	}

	List<TheGameObject> GetAllianceShips()
	{
		return alliance_ships;
	}

	List<TheGameObject> GetEmpireShips()
	{
		return empire_ships;
	}	

	TheGameObject GetSlave1()
	{
		return slave1;
	}
	
	List<TheGameObject> GetTurrets()
	{
		return turret_entities;
	}

	List<TheGameObject> GetGenerators()
	{
		return generator_entities;
	}

	int AllianceShipsCount()
	{
		return alliance_ships.Count;
	}
	
	int EmpireShipsCount()
	{
		return empire_ships.Count;
	}

	int TurretsCount()
	{
		return turret_entities.Count;
	}

	int GeneratorsCount()
	{
		return generator_entities.Count;
	}

	void CallTrigger(string trigger_name, TheGameObject go_triggerer)
	{
		if(is_level1)
		{
			if(level1_script != null)
			{
				object[] args = {trigger_name, go_triggerer};
				level1_script.CallFunctionArgs("CallTrigger", args);
			}
		}
	}
	
	void AddToRadar(TheGameObject add, string marker_type)
	{		
		if(front_radar != null)
		{
			front_radar.AddEntity(add);
			front_radar.SetMarkerToEntity(add, marker_type);
			TheConsole.Log("Adding to front radar");
		}
		else
			TheConsole.Log("radar is null");
	
		if(back_radar != null)
		{
			back_radar.AddEntity(add);
			back_radar.SetMarkerToEntity(add, marker_type);
			TheConsole.Log("Adding to back radar");
		}
	}

	void RemoveFromRadar(TheGameObject remove)
	{
		if(front_radar != null)
			front_radar.RemoveEntity(remove);
	
		if(back_radar != null)
			back_radar.RemoveEntity(remove);
	}
	/*
	void UpdateAudio()
	{
	
		if (TheInput.GetControllerButton(0,"CONTROLLER_A") == 2 && !calm_combat)
		{
			if(audio_source != null)
			{
				audio_source.SetVolume(combat_volume);
				audio_source.SetState("Level","Combat");
			}
			calm_combat = true;
		}
		
		if (calm_combat)
		{
			background_music_timer -= TheTime.DeltaTime;
			
			if(background_music_timer < 0.0f)
			{	
				if(audio_source != null)
				{
					audio_source.SetVolume(calm_volume);
					audio_source.SetState("Level","Calm");
				}

				background_music_timer = music_timer;

				calm_combat = false;
			}
		}
	}
	*/
}