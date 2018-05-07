using TheEngine;
using TheEngine.TheConsole; 
using System.Collections.Generic;

public class TrainingModeManager 
{
	bool enabled = false;	

	public string ship_prefab_name;
	public float max_spawn_rad = 50;
	public float min_spawn_rad = 30;

	public TheGameObject slave_emmiter;
	TheAudioSource slave_audio;
	public TheGameObject curr_wave_go;
	public TheGameObject remaining_ships_go;
	TheText curr_wave_text = null;
	TheText remaining_ships_text = null;
	
	TheScript game_manager_script = null;

	TheTimer check_wave_finished_timer = new TheTimer();

	int curr_wave = 0;
	int ships_remaining_to_spawn = 0;
	TheTimer spawn_timer = new TheTimer();

	TheScript slave1_script = null;

	TheTimer check_win_lose = new TheTimer();

	public TheGameObject gametime_go;
	private TheText gametime_text = null; 
	public TheGameObject score_go; 
	private TheText score_text = null; 

	private TheTimer game_timer = new TheTimer();

	private int score = 0; 
	
	TheAudioSource audio_source;
	void Init()
	{
		TheGameObject game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");

		if(curr_wave_go != null)
			curr_wave_text = curr_wave_go.GetComponent<TheText>();

		if(remaining_ships_go != null)
			remaining_ships_text = remaining_ships_go.GetComponent<TheText>();

		if(gametime_go != null)
	 		gametime_text = gametime_go.GetComponent<TheText>();

		if(score_go != null)
			score_text = score_go.GetComponent<TheText>();
		
		if(gametime_text != null)
			gametime_text.Text = GetTimeFromSeconds(0); 

		if(score_text != null)
			score_text.Text = score.ToString();
		if(slave_emmiter!=null)
			slave_audio = slave_emmiter.GetComponent<TheAudioSource>();

		audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();
	}

	void Start () 
	{
		if(game_manager_script != null)
		{
			enabled = (bool)game_manager_script.CallFunctionArgs("GetIsTrainingMode");

			if(enabled)
				TheConsole.Log("Training Mode activated! :D");

			check_wave_finished_timer.Start();
			spawn_timer.Start();
			check_win_lose.Start();

			TheGameObject slave1 = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");

			if(slave1 != null)
				slave1_script = slave1.GetScript("EntityProperties");
		}

		game_timer.Start();

		if(audio_source != null)
		{
			audio_source.Play("Play_Music");
			audio_source.SetState("Level","Training");

		}
	}
	
	void Update () 
	{
		if(enabled)
		{	
			CheckWinLose();

			if(CheckWaveFinished())
			{
				SpawnNextWave();
			}

			SpawnRemainingShips();

			UpdateTimePointsTexts();
		}
	}

	void CheckWinLose()
	{
		if(check_win_lose.ReadTime() > 1)
		{
			if(slave1_script != null)
			{
				bool dead = (bool)slave1_script.CallFunctionArgs("IsDead");

				if(dead)
				{
					if(slave_audio!=null)
						slave_audio.Play("Stop_Engine");
					Lose();
				}
			}	

			check_win_lose.Start();
		}
	}

	void Lose()
	{
		TheData.AddString("score", score_text.Text);
		TheData.AddString("time", gametime_text.Text);
		TheData.AddString("faction", "rebels");
		TheData.AddString("mode", "training");
		TheData.AddInt("won", 0);
		TheApplication.LoadScene("Alpha1 - EndGameScene");
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

	string GetTimeFromSeconds(int seconds)
	{
		int division = seconds/60; 
		float reminder = seconds % 60;

		string new_time = division.ToString(); 
		new_time += ":";

		if(reminder < 10) 
			new_time += "0";

		new_time += reminder.ToString(); 
		 
		return new_time; 
	}

	int GetTimeSinceStart()
	{
		return (int)game_timer.ReadTime();
	}

	void AddScore(int add)
	{
		if(add > 0)
		{
			score += add;
		}
	}
	
	void UpdateTimePointsTexts()
	{		
		int remaining = GetTimeSinceStart();

		if(gametime_text != null)
			gametime_text.Text = GetTimeFromSeconds(remaining); 

		if(score_text != null)
			score_text.Text = score.ToString();
	}

	void OnShipDestroyedCallback(TheGameObject ship, TheGameObject killer)
	{
		if(killer != null && ship != null)
		{
			TheScript killer_script = killer.GetScript("EntityProperties");
			
			if(killer_script != null)
			{
				bool is_slave = (bool)killer_script.CallFunctionArgs("IsSlave1");

				if(is_slave)
				{
					AddScore(100);
				}
			}
		}
	}
}