using TheEngine;
using System.Collections.Generic; 
using TheEngine.TheConsole; 

public class GameManager
{
	private string team = "no_str"; 

	public int game_time = 120;
	private TheTimer game_timer = new TheTimer();

	private int score = 0;

	public TheGameObject gametime_go;
	private TheText gametime_text = null; 
	public TheGameObject score_go; 
	private TheText score_text = null; 
	
	public TheGameObject alliance_spawn;
	TheTransform alliance_spawn_trans = null;
    public TheGameObject empire_spawn;
	TheTransform empire_spawn_trans = null;
	
	public float music_timer = 15;
	private float background_music_timer = 0.0f;
	private bool calm_combat = false;
	
	public int calm_volume = 10;
	public int combat_volume = 10;

	TheAudioSource audio_source = null;
	
	public TheGameObject slave1_audio;
	TheTransform slave1_trans = null;
	TheAudioSource slave1_audiosource = null;

	public TheGameObject front_radar_go;
	TheRadar front_radar = null;
	public TheGameObject back_radar_go;
	TheRadar back_radar = null;

	List<TheGameObject> alliance_ships = new List<TheGameObject>();
    List<TheGameObject> empire_ships = new List<TheGameObject>();
	TheGameObject slave1 = null;
	
	string alliance_name = "alliance";
	string empire_name = "empire";

	bool is_training_mode = false;

	void Init ()
	{
		team = TheData.GetString("faction");

		if(team == "no_str")
			team = "alliance";

		is_training_mode = System.Convert.ToBoolean(TheData.GetInt("is_training_mode"));

        TheConsole.Log("Team: " + team); 

		if(is_training_mode)
			TheConsole.Log("Training mode enabled!"); 
	}

	void Start ()
	{
		if(slave1 != null)
			slave1_trans = slave1.GetComponent<TheTransform>();
				
		if(alliance_spawn != null)
			alliance_spawn_trans = alliance_spawn.GetComponent<TheTransform>();
		
		if(empire_spawn != null)
			empire_spawn_trans = empire_spawn.GetComponent<TheTransform>();

		if(gametime_go != null)
	 		gametime_text = gametime_go.GetComponent<TheText>();

		if(score_go != null)
			score_text = score_go.GetComponent<TheText>();
		
		if(gametime_text != null)
			gametime_text.Text = GetTimeFromSeconds(game_time); 

		if(score_text != null)
			score_text.Text = score.ToString();

		if(front_radar_go != null)
			front_radar = front_radar_go.GetComponent<TheRadar>();

		if(back_radar_go != null)
			back_radar = back_radar_go.GetComponent<TheRadar>();

		audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();

		if(slave1_audio != null)
			slave1_audiosource = slave1_audio.GetComponent<TheAudioSource>();
		
		if(audio_source != null)
		{
			audio_source.Play("Play_Music");
			audio_source.SetVolume(calm_volume);
		}
			
		background_music_timer = music_timer;

		game_timer.Start();
	}
	
	void Update () 
	{
		UpdateAudio();

		UpdateTimePointsTexts();

		CheckWinLose();
	}

	void CheckWinLose()
	{
		/*
		if(GetRemainingTime() <= 0)
		{
			Lose();
		}
		*/
		/*else if()
		{
			Win();
		}
		*/
	}

	bool GetIsTrainingMode()
	{
		return is_training_mode;
	}

	void Win()
	{
		TheConsole.Log("You win!");
		TheApplication.LoadScene("VS4 - MainMenu");
	}

	void Lose()
	{
		TheConsole.Log("You lose!");
		TheApplication.LoadScene("VS4 - MainMenu");
	}

	void AddScore(int add)
	{
		score += add;
	}
	
	void AddSlave1(TheGameObject slave)
	{
		slave1 = slave;

		if(slave != null)
		{
			TheScript slave1_properties = slave.GetScript("ShipProperties");
	
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

	void AddAllianceShip(TheGameObject add)
	{
		if(add != null)
		{
			TheScript ship_properties = add.GetScript("ShipProperties");

			if(ship_properties != null)
			{	
				bool is_slave = (bool)ship_properties.CallFunctionArgs("IsSlave1");		
				if(!is_slave)
				{
					alliance_ships.Add(add);

					if(front_radar != null)
					{
						front_radar.AddEntity(add);
						front_radar.SetMarkerToEntity(add, "Alliance");
					}
		
					if(back_radar != null)
					{
						back_radar.AddEntity(add);
						back_radar.SetMarkerToEntity(add, "Alliance");
					}
					

					TheConsole.Log("Ship added to alliance!: " + AllianceShipsCount());
				}
			}
		}
	}

	void AddEmpireShip(TheGameObject add)
	{
		if(add != null)
		{
			TheScript ship_properties = add.GetScript("ShipProperties");
			
			if(ship_properties != null)
			{
				bool is_slave = (bool)ship_properties.CallFunctionArgs("IsSlave1");		
				if(!is_slave)
				{
					empire_ships.Add(add);
				
					if(front_radar != null)
					{
						front_radar.AddEntity(add);
						front_radar.SetMarkerToEntity(add, "Empire");
					}

					if(back_radar != null)
					{
						back_radar.AddEntity(add);
						back_radar.SetMarkerToEntity(add, "Alliance");
					}

					TheConsole.Log("Ship added to empire!: " + EmpireShipsCount());
				}
			}
		}
	}


	void RemoveShip(TheGameObject remove)
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
				TheScript ship_prop = alliance_ships[i].GetScript("ShipProperties");
				if(ship_prop != null)
				{
					object[] args = {remove};
					ship_prop.CallFunctionArgs("OnShipDestroyedCallback", args);
				}
			}

			for(int i = 0; i < empire_ships.Count; ++i)
			{
				TheScript ship_prop = empire_ships[i].GetScript("ShipProperties");
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

		}
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
	
	int AllianceShipsCount()
	{
		return alliance_ships.Count;
	}
	
	int EmpireShipsCount()
	{
		return empire_ships.Count;
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

	int GetRemainingTime()
	{
		int remaining = (int)((float)game_time - game_timer.ReadTime());

		if(remaining < 0)
			remaining = 0;

		return remaining;
	}

	int GetTimeSinceStart()
	{
		return (int)game_timer.ReadTime();
	}

	void UpdateTimePointsTexts()
	{		
		int remaining = GetTimeSinceStart();

		if(gametime_text != null)
			gametime_text.Text = GetTimeFromSeconds(remaining); 

		if(score_text != null)
			score_text.Text = score.ToString();
	}

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
}