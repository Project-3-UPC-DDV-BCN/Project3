using TheEngine;
using TheEngine.TheConsole;
using System.Collections.Generic;

public class TrainingModeManager 
{
	bool enabled = false;	

	public string ship_prefab_name;
	
	TheScript game_manager_script = null;

	TheTimer check_wave_finished_timer = new TheTimer();

	List<TheTransform> spawners = new List<TheTransform>();
	int curr_wave = 0;

	int ships_remaining_to_spawn = 0;

	TheTimer spawn_timer = new TheTimer();

	void Init()
	{
		TheGameObject game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");
	}

	void Start () 
	{
		if(game_manager_script != null)
		{
			enabled = (bool)game_manager_script.CallFunctionArgs("GetIsTrainingMoode");
			
			//Temporal for VS4 testing
			enabled = true;

			if(enabled)
				TheConsole.Log("Training mode enabled!"); 

			check_wave_finished_timer.Start();
		}
	}
	
	void Update () 
	{
		if(enabled)
		{
			if(CheckWaveFinished())
			{
				SpawnNextWave();
			}

			SpawnRemainingShips();
		}
	}

	bool CheckWaveFinished()
	{
		bool ret = false;

		if(check_wave_finished_timer.ReadTime() > 1.5f && ships_remaining_to_spawn == 0)
		{
			if(game_manager_script != null)
			{
				int enemies_count = (int)game_manager_script.CallFunctionArgs("GetSlaveEnemiesCount");
		
				if(enemies_count == 0)
					ret = true;
			}

			check_wave_finished_timer.Start();
		}

		return ret;
	}

	void SpawnNextWave()
	{
		++curr_wave;

		ships_remaining_to_spawn = curr_wave;

		spawn_timer.Start();
	}

	void SpawnRemainingShips()
	{
		if(ships_remaining_to_spawn > 0 && spawners.Count > 0)
		{
			if(spawn_timer.ReadTime() > 2)
			{
				TheTransform rand_spawner = spawners[(int)TheRandom.RandomRange(0, spawners.Count - 1)];

				TheGameObject spawned_ship = TheResources.LoadPrefab(ship_prefab_name);

				if(spawned_ship != null && rand_spawner != null)
				{
					TheTransform spawned_ship_trans = spawned_ship.GetComponent<TheTransform>();

					spawned_ship_trans.GlobalPosition = rand_spawner.GlobalPosition;

					TheConsole.Log("Enemy ship spawned!");

					if(game_manager_script != null)
					{
						object[] args = {spawned_ship};
						game_manager_script.CallFunctionArgs("AddSlaveEnemiesShip", args);
					}
				}

				spawn_timer.Start();
				ships_remaining_to_spawn--;
			}
		}
	}
}