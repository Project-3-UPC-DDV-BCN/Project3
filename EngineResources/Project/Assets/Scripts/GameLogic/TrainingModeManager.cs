using TheEngine;
using TheEngine.TheConsole;
using System.Collections.Generic;

public class TrainingModeManager 
{
	bool enabled = false;	

	public string ship_prefab_name;
	public float max_spawn_rad = 50;
	public float min_spawn_rad = 30;

	public TheGameObject curr_wave_go;
	public TheGameObject remaining_ships_go;
	TheText curr_wave_text = null;
	TheText remaining_ships_text = null;
	
	TheScript game_manager_script = null;

	TheTransform scene_center = null;

	TheTimer check_wave_finished_timer = new TheTimer();

	int curr_wave = 0;

	int ships_remaining_to_spawn = 0;

	TheTimer spawn_timer = new TheTimer();

	void Init()
	{
		TheGameObject game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");

		TheGameObject[] anchors = TheGameObject.GetGameObjectsWithTag("AI_ANCHOR");
		if(anchors.Length > 0)
		{
			TheGameObject anchor = anchors[0];
			if(anchor != null)
			{
				scene_center = anchor.GetComponent<TheTransform>();
			}
		}

		if(curr_wave_go != null)
			curr_wave_text = curr_wave_go.GetComponent<TheText>();

		if(remaining_ships_go != null)
			remaining_ships_text = remaining_ships_go.GetComponent<TheText>();
	}

	void Start () 
	{
		if(game_manager_script != null)
		{
			enabled = (bool)game_manager_script.CallFunctionArgs("GetIsTrainingMode");
			
			//Temporal for VS4 testing
			enabled = true;

			if(enabled)
				TheConsole.Log("Training mode enabled!"); 

			check_wave_finished_timer.Start();
			spawn_timer.Start();
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

				SetRemainingShipsText(enemies_count);
		
				if(enemies_count == 0)
				{
					TheConsole.Log("No enemies! Need to spawn new round");
					ret = true;
				}
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

		TheConsole.Log("Spawning wave: " + curr_wave);

		SetCurrWaveText(curr_wave);
	}

	void SpawnRemainingShips()
	{
		if(ships_remaining_to_spawn > 0)
		{
			if(spawn_timer.ReadTime() > 2)
			{
				TheGameObject spawned_ship = TheResources.LoadPrefab(ship_prefab_name);

				if(spawned_ship != null)
				{
					TheTransform spawned_ship_trans = spawned_ship.GetComponent<TheTransform>();

					spawned_ship_trans.GlobalPosition = GetRandomSpawnPoint();
			
					TheVector3 spawned_pos = spawned_ship_trans.GlobalPosition;
					TheConsole.Log("Enemy ship spawned! x:" + spawned_pos.x + " y: " + spawned_pos.y + " z:" + spawned_pos.z);
				}

				spawn_timer.Start();
				ships_remaining_to_spawn--;
			}
		}
	}

	TheVector3 GetRandomSpawnPoint()
	{
		TheVector3 ret = new TheVector3(0, 0, 0);					
		
		float x_add = TheRandom.RandomRange(min_spawn_rad, max_spawn_rad);
		float y_add = TheRandom.RandomRange(min_spawn_rad, max_spawn_rad);
		float z_add = TheRandom.RandomRange(min_spawn_rad, max_spawn_rad);

		if((int)TheRandom.RandomRange(1, 3) > 2)
			x_add = -x_add;

		if((int)TheRandom.RandomRange(1, 3) > 2)
			y_add = -y_add;

		if((int)TheRandom.RandomRange(1, 3) > 2)
			z_add = -z_add;
		
			
		if(scene_center != null)
			ret = scene_center.GlobalPosition;			
			
		ret.x += x_add;
		ret.y += y_add;
		ret.z += z_add;

		return ret;
	}

	void SetCurrWaveText(int wave)
	{
		if(curr_wave_text != null)
		{
			curr_wave_text.Text = "Current wave: " + wave.ToString();
		}
	}

	void SetRemainingShipsText(int remaining_ships)
	{
		if(remaining_ships_text != null)
		{
			remaining_ships_text.Text = "Enemies: " + remaining_ships.ToString();
		}
	}
}