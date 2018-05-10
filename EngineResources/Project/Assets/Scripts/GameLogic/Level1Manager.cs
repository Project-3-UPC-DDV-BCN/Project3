using TheEngine;
using TheEngine.TheConsole;
using System.Collections.Generic;

public class Level1Manager 
{
	private bool enabled = false;	

	TheGameObject self = null;

	TheScript game_manager_script = null;

	// Slave1
	TheGameObject slave1 = null;
	TheTransform slave_trans = null;

	public TheGameObject slave_emmiter;
	TheAudioSource slave_audio;
	TheScript slave1_script = null;
	TheScript slave1_movement_script = null;
	TheScript slave1_shooting_script = null;

	// UI
	public TheGameObject mission_state_background_go;
	public TheGameObject mission_state_text_go;
	TheText mission_state_text = null;
	public TheGameObject enemies_to_kill_background_go;
	public TheGameObject enemies_to_kill_text_go;
	TheText enemies_to_kill_text = null;
	public TheGameObject ackbar_canvas_go = null;
	public TheGameObject ackbar_text_go = null;
	TheText ackbar_text = null;
	public TheGameObject enemies_canvas_go = null;
	public TheGameObject enemies_text_go = null;
	TheText enemies_text = null;

	TheScript dialog_manager = null;

	// Auido
	TheAudioSource audio_source = null;
	public float music_distance = 3000;
	public TheGameObject fight_zone;
	TheTransform fight_trans = null;
	

	int curr_mission_state = 0;
	// 1 - Slave 1 cant move. Receives commands from General Ackbar
	// 2 - TIE-Fighter appears from the right of the player. 
	//	   Follows path into asteroid hole and stops some meters in front of the exit.
	// 3 - Ackbar intro briefing and see ring. Receives commands from General Ackbar
	// 4 - Players follow the ship into the asteroid. In the middle of it, 
    //     they can see the ship stopped in front. Receives commands from General Ackbar
	// 5 - 2 enemy ships appear from the sides, start coming towards the player
    //     (if they are not destroyed, they start going to the shield gate and disappear after 6 seconds of not being hit).
	// 6 - Player destroys the TIE FIGHTER in front (and maybe the other 2). 
	// 7 - Receives commands from General Ackbar
	// 8 - Players go through asteroid field.
	// 9 - When they reach the final one (purple line more or less), players receive 
    //     communications from the enemy (interferences of their radio): Intruder in our base!...
    //     (radio noise)...All units ready!....”
	// 10 - 3 ships in the close part of the shield gate appear and start patrolling.
    // 11 - At the same time, the lights of energy of the shield generators lit up.
	// 12 - When the 3 ships are destroyed or after 1 minute, another squad appears from
    //      random positions near the shield generators and try to destroy the player.
	// 13 - When the players destroy the first shield generator, 
	//	    General Ackbar says: Fast! We need the shields down!”

	List<TheGameObject> entities_to_destroy = new List<TheGameObject>();

	// Ships intro
	TheGameObject intro_ship = null;
	public TheGameObject intro_ship_spawn;
	public TheGameObject intro_ship_path;

	public TheGameObject intro_ship_spawn_2;
	public TheGameObject intro_ship_spawn_3;

	public string intro_enemy_ship_prefab;

	private TheTimer attack_intro_ship = new TheTimer();
	
	// Main mission ships
	public string enemy_ship_prefab;

	public TheGameObject main_ship_spawner1;
	TheTransform main_ship_spawner1_trans = null;

	public TheGameObject main_ship_spawner2;
	TheTransform main_ship_spawner2_trans = null;

	public TheGameObject main_ship_spawner3;
	TheTransform main_ship_spawner3_trans = null;

	public TheGameObject main_ship_spawner4;
	TheTransform main_ship_spawner4_trans = null;

	public TheGameObject main_ship_spawner5;
	TheTransform main_ship_spawner5_trans = null;

	public TheGameObject main_ship_spawner6;
	TheTransform main_ship_spawner6_trans = null;

	public string destroyer_ship_prefab;
	public TheGameObject destroyer_ship_spawner;
	TheTransform destroyer_ship_spawner_trans = null;

	// Spawn
	int ships_to_spawn = 0;
	TheTimer timer_between_spawn = new TheTimer();
	float time_between_spawn = 3.5f;
	TheTimer new_spawn_timer = new TheTimer();
	float time_between_new_spawn = 50.0f;

	// Warp
	TheTimer time_to_warp = new TheTimer();
	int time_to_survive = 20;
	
	TheTimer warp_prepare_time = new TheTimer();
	TheTimer warp_time = new TheTimer();
	bool warping = false;
	bool preparing_warp = false;

	TheTimer check_win_lose = new TheTimer();

	void Init()
	{
		self = TheGameObject.Self;

		TheGameObject game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");

		dialog_manager = self.GetScript("DialogManager");
	
		if(mission_state_text_go != null)
			mission_state_text = mission_state_text_go.GetComponent<TheText>();

		if(enemies_to_kill_text_go != null)
			enemies_to_kill_text = enemies_to_kill_text_go.GetComponent<TheText>();

		if(ackbar_canvas_go != null)
			ackbar_canvas_go.SetActive(false);

		if(ackbar_text_go != null)
			ackbar_text = ackbar_text_go.GetComponent<TheText>();

		if(enemies_canvas_go != null)
			enemies_canvas_go.SetActive(false);

		if(enemies_text_go != null)
			enemies_text = enemies_text_go.GetComponent<TheText>();

		if(fight_zone != null)
			fight_trans = fight_zone.GetComponent<TheTransform>();

		if(slave_emmiter!=null)
			slave_audio = slave_emmiter.GetComponent<TheAudioSource>();

		if(main_ship_spawner1 != null)
			main_ship_spawner1_trans = main_ship_spawner1.GetComponent<TheTransform>();
	
		if(main_ship_spawner2 != null)
			main_ship_spawner2_trans = main_ship_spawner2.GetComponent<TheTransform>();

		if(main_ship_spawner3 != null)
			main_ship_spawner3_trans = main_ship_spawner3.GetComponent<TheTransform>();
	
		if(main_ship_spawner4 != null)
			main_ship_spawner4_trans = main_ship_spawner4.GetComponent<TheTransform>();

		if(main_ship_spawner5 != null)
			main_ship_spawner5_trans = main_ship_spawner5.GetComponent<TheTransform>();
	
		if(main_ship_spawner6 != null)
			main_ship_spawner6_trans = main_ship_spawner6.GetComponent<TheTransform>();

		audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();
	}

	void Start () 
	{
		if(game_manager_script != null)
		{
			enabled = (bool)game_manager_script.CallFunctionArgs("GetIsLevel1");

			if(enabled)
				TheConsole.Log("Level1 activated! :D");
		}
		
		slave1 = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");
		if(slave1 != null)
		{
			slave1_script = slave1.GetScript("EntityProperties");
			slave1_movement_script = slave1.GetScript("PlayerMovement");
			slave_trans = slave1.GetComponent<TheTransform>();

			TheGameObject[] childs = slave1.GetAllChilds();

			for(int i = 0; i < childs.Length; ++i)
			{
				slave1_shooting_script = childs[i].GetScript("VS4StarShipShooting");

				if(slave1_shooting_script != null)
					break;
			}
		}

		if(audio_source!= null)
			audio_source.Play("Play_Music");

		// Dialogs
		if(dialog_manager != null)
		{
			TheConsole.Log("Setting dialogs");

			object[] args1 = {"AckbarIntro1"};
			dialog_manager.CallFunctionArgs("NewDialog", args1);

			object[] args2 = {"AckbarIntro1", "Bobba, you are behind enemy lines.", 6.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args2);

			object[] args3 =  {"AckbarIntro1", "Be careful, try not to get detected", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args3);

			object[] args8 =  {"AckbarIntro1", "or things will heat up in no time!", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args8);


			object[] args9 = {"AckbarIntro2"};
			dialog_manager.CallFunctionArgs("NewDialog", args9);

			object[] args10 = {"AckbarIntro2", "Look, there is an enemy ship at your right!", 5.0f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args10);


			object[] args5 = {"AckbarIntro3"};
			dialog_manager.CallFunctionArgs("NewDialog", args5);

			object[] args6 = {"AckbarIntro3", "Don't get trigger happy, Bobba.", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args6);

			object[] args7 = {"AckbarIntro3", "Follow that ship. Let's see where this leads to...", 6.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args7);


			object[] args11 = {"AckbarAttack1"};
			dialog_manager.CallFunctionArgs("NewDialog", args11);

			object[] args12 = {"AckbarAttack1", "Oh no, they found you Bobba! Don't hesitate!,", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args12);

			object[] args13 = {"AckbarAttack1", "Shoot'em down!", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args13);


			object[] args14 = {"IntroSucces"};
			dialog_manager.CallFunctionArgs("NewDialog", args14);

			object[] args15 = {"IntroSucces", "Well done, Bobba. But it ain't finished.", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args15);

			object[] args16 = {"IntroSucces", "Go to the shield gate and destroy all the generators", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args16);

			object[] args17 = {"IntroSucces", "so our fleet can destroy the ship!", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args17);

			
			object[] args20 = {"EnemiesInterception"};
			dialog_manager.CallFunctionArgs("NewDialog", args20);

			object[] args21 = {"EnemiesInterception", "...Intruders in our base!...", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args21);

			object[] args22 = {"EnemiesInterception", "...All units ready!...", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args22);


			object[] args27 = {"HalfMission"};
			dialog_manager.CallFunctionArgs("NewDialog", args27);

			object[] args25 = {"HalfMission", "We are running out of time!", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args25);

			object[] args26 = {"HalfMission", "We need the shields down", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args26);
			

			object[] args23 = {"WinDialog"};
			dialog_manager.CallFunctionArgs("NewDialog", args23);

			object[] args24 = {"WinDialog", "Great work Bobba! You did it!", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args24);

			object[] args30 = {"WinDialog", "But you need time to clear the way for you to jump into hyperspace", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args30);

			object[] args28 = {"WinDialog", "to jump into hyperspace.", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args28);

			object[] args29 = {"WinDialog", "Survive 20 seconds!", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args29);


			object[] args31 = {"EndDialog"};
			dialog_manager.CallFunctionArgs("NewDialog", args31);

			object[] args32 = {"EndDialog", "Well done! Let's get out of here.", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args32);

		}

		HideCurrMissionObj();
		HideEnemiesToKill();

		// Start mission
		NextMissionState();

		audio_source.SetState("Level","Calm");

		new_spawn_timer.Start();
		
		check_win_lose.Start();
	}
	
	void Update () 
	{
		UpdateMissionState(curr_mission_state);
		
		CheckWinLose();
	}

	void UpdateAudio()
	{
		if(slave_trans!=null)
		{
			float distance = TheVector3.Distance(fight_trans.GlobalPosition, slave_trans.GlobalPosition);

			if(distance < music_distance)
			{
				if(audio_source != null)
					audio_source.SetState("Level","Combat");
			}
			else
			{
				if(audio_source != null)
					audio_source.SetState("Level","Calm");
			}
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
					{
						slave_audio.Play("Stop_Engine");
						audio_source.Stop("Play_Music");
					}
					Lose();
				}
			}	

			check_win_lose.Start();
		}
	}

	void Lose()
	{
		TheData.AddString("score", "0");
		TheData.AddString("time", "0");
		TheData.AddString("mode", "campaign");
		TheData.AddInt("won", 0);

		TheApplication.LoadScene("Alpha1 - EndGameSceneCampaign");
	}

	void Win()
	{
		if(slave_audio!=null)
		{
			slave_audio.Play("Stop_Engine");
			audio_source.Stop("Play_Music");
		}

		TheData.AddString("score", "0");
		TheData.AddString("time", "0");
		TheData.AddString("mode", "campaign");
		TheData.AddInt("won", 1);

		TheApplication.LoadScene("Alpha1 - EndGameScene");
	}

	void NextMissionState()
	{
		FinishMissionState(curr_mission_state);

		++curr_mission_state;

		StartMissionState(curr_mission_state);
	}

	void StartMissionState(int state)
	{
		switch(state)
		{
			case 1:
			{
				if(dialog_manager != null)
				{
					object[] args = {ackbar_canvas_go};
					dialog_manager.CallFunctionArgs("SetCanvas", args);

					object[] args0 = {ackbar_text};
					dialog_manager.CallFunctionArgs("SetTextComponent", args0);

					object[] args2 =  {"AckbarIntro1"};
					dialog_manager.CallFunctionArgs("FireDialog", args2);
				}

				if(slave1_movement_script != null)
				{
					object[] args = {false};
					slave1_movement_script.CallFunctionArgs("SetCanMove", args);
				}

				if(slave1_shooting_script != null)
				{
					object[] args = {false};
					slave1_shooting_script.CallFunctionArgs("SetCanShoot", args);
				}

				break;
			}
			case 2:
			{
				SpawnIntroShip();

				if(dialog_manager != null)
				{
					object[] args =  {"AckbarIntro2"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}

				SetCurrMissionObj("Don't get spotted");

				attack_intro_ship.Start();
				
				break;
			}
			case 3:
			{
				if(slave1_movement_script != null)
				{
					object[] args = {true};
					slave1_movement_script.CallFunctionArgs("SetCanMove", args);
				}

				if(dialog_manager != null)
				{
					object[] args =  {"AckbarIntro3"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}

				break;
			}
			case 4:
			{
				if(audio_source != null)
					audio_source.SetState("Level","Combat");

				SetCurrMissionObj("Survive the ambush");
			
				if(slave1_shooting_script != null)
				{
					object[] args = {true};
					slave1_shooting_script.CallFunctionArgs("SetCanShoot", args);
				}

				if(dialog_manager != null)
				{
					object[] args =  {"AckbarAttack1"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}

				break;
			}
			case 5:
			{
				if(audio_source != null)
					audio_source.SetState("Level","Calm");

				SetCurrMissionObj("Approach the shield gate");

				if(dialog_manager != null)
				{
					object[] args =  {"IntroSucces"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}

				// Adding generators to radar
				if(game_manager_script != null)
				{
					List<TheGameObject> generators = (List<TheGameObject>)game_manager_script.CallFunctionArgs("GetGenerators");

					for(int i = 0; i < generators.Count; ++i)
					{
						object[] args = {generators[i], "Alliance"};
						game_manager_script.CallFunctionArgs("AddToRadar", args);

						AddToEntitiesToDestroy(generators[i]);
					}
				}

				break;
			}
			case 6:
			{
				if(audio_source != null)
					audio_source.SetState("Level","Combat");

				SetCurrMissionObj("Destroy all generators");

				SpawnNextWave(5);
				
				if(dialog_manager != null)
				{
					object[] args =  {"EnemiesInterception"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}
				
				timer_between_spawn.Start();
			

				break;
			}
			case 7:
			{
				if(dialog_manager != null)
				{
					object[] args =  {"HalfMission"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}
	
				break;
			}
			case 8:
			{
				time_to_warp.Start();

				SetCurrMissionObj("Survive until hyperspace jump is ready");

				if(dialog_manager != null)
				{
					object[] args =  {"WinDialog"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}
				break;
			}
			case 9:
			{
				if(slave1_movement_script != null)
				{
					object[] args = {false};
					slave1_movement_script.CallFunctionArgs("SetCanMove", args);
				}

				if(dialog_manager != null)
				{
					object[] args =  {"EndDialog"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}
				break;
			}
		}
	}

	void FinishMissionState(int state)
	{
		switch(state)
		{
			case 1:
			{
				break;
			}
			case 2:
			{
				break;
			}
			case 3:
			{
				if(intro_ship != null)
				{
					TheScript movement_script = intro_ship.GetScript("GuillemMovement");

					object[] args = {0};
					movement_script.CallFunctionArgs("SetMovementMode", args);
				}

				SpawnAmbushShips();
				
				break;
			}
			case 4:
			{
				HideEnemiesToKill();
				break;
			}
			case 5:
			{				
				if(dialog_manager != null)
				{
					object[] args = {enemies_canvas_go};
					dialog_manager.CallFunctionArgs("SetCanvas", args);

					object[] args0 = {enemies_text};
					dialog_manager.CallFunctionArgs("SetTextComponent", args0);	
				}
	
				break;
			}
			case 6:
			{
				if(dialog_manager != null)
				{
					object[] args = {ackbar_canvas_go};
					dialog_manager.CallFunctionArgs("SetCanvas", args);

					object[] args0 = {ackbar_text};
					dialog_manager.CallFunctionArgs("SetTextComponent", args0);
				}

				break;	
			}
			case 7:
			{
				break;
			}
			case 8:
			{
				HideCurrMissionObj();
				HideEnemiesToKill();

				break;
			}
			
		}
	}

	void UpdateMissionState(int state)
	{
		switch(state)
		{
			case 1:
			{
				if(dialog_manager != null)
				{
					bool running = (bool)dialog_manager.CallFunctionArgs("DialogIsRunning");
		
					if(!running)
					{
						NextMissionState();
					}
				}

				break;
			}
			case 2:
			{
				if(attack_intro_ship.ReadTime() > 9)
					NextMissionState();
				
				break;
			}
			case 3:
			{
				break;
			}
			case 4:
			{
				SetEnemiesToKill("Ships to destroy: " + GetEntitiesToDestroyCount().ToString());
				
				if(GetEntitiesToDestroyCount() == 0)
					NextMissionState();
					
				break;
			}
			case 5:
			{
				break;
			}
			case 6:
			{
				SetEnemiesToKill("Generators to destroy: " + GetEntitiesToDestroyCount().ToString());

				SpawnMainMissionShips();

				if(new_spawn_timer.ReadTime() > time_between_new_spawn)
				{
					SpawnNextWave(5);
					new_spawn_timer.Start();
				}

				if(GetEntitiesToDestroyCount() <= 3)
				{
					bool running = false;

					if(dialog_manager != null)
					{
						running = (bool)dialog_manager.CallFunctionArgs("DialogIsRunning");
					}

					if(!running)
						NextMissionState();
				}

				break;
			}
			case 7:
			{
				SetEnemiesToKill("Generators to destroy: " + GetEntitiesToDestroyCount().ToString());

				SpawnMainMissionShips();

				if(new_spawn_timer.ReadTime() > time_between_new_spawn)
				{
					SpawnNextWave(5);
					new_spawn_timer.Start();
				}

				if(GetEntitiesToDestroyCount() == 0)
				{
					NextMissionState();
				}
				

				break;
			}
			case 8:
			{
				SetEnemiesToKill("Time to survive: " + (time_to_survive - (int)time_to_warp.ReadTime()).ToString());
	
				if(time_to_warp.ReadTime() > time_to_survive)
				{
					NextMissionState();
				}
				
				break;
			}
			case 9:
			{
				if(dialog_manager != null)
				{
					bool running = (bool)dialog_manager.CallFunctionArgs("DialogIsRunning");

					if(!running && !preparing_warp)
					{
						warp_prepare_time.Start();
						preparing_warp = true;
					}

					if(warp_prepare_time.ReadTime() > 3 && !warping)
					{
						warp_time.Start();
						warping = true;
					}

					if(warping && warp_time.ReadTime() < 4)
					{
						TheVector3 speed_dir = new TheVector3(0, 0, 0);
						speed_dir = slave_trans.ForwardDirection;
						speed_dir *= warp_time.ReadTime() * TheTime.DeltaTime * 300;

						slave_trans.LocalPosition += speed_dir;
					}
					else if(warping &&  warp_time.ReadTime() > 4)
					{
						Win();
					}
					
				}

				break;
			}
		}
	}

	void CallTrigger(string trigger_name, TheGameObject go_triggerer)
	{
		if(trigger_name == "IntroTrigger")
		{
			if(go_triggerer == slave1 && curr_mission_state == 3)
			{
				NextMissionState();
			}
		}

		else if(trigger_name == "MainMissionTrigger")
		{
			if(go_triggerer == slave1 && curr_mission_state == 5)
			{
				NextMissionState();
			}
		}
	}

	void OnShipDestroyedCallback(TheGameObject ship, TheGameObject killer)
	{
		RemoveFromEntitiesToDestroy(ship);
	}

	void OnTurretDestroyedCallback(TheGameObject ship, TheGameObject killer)
	{

	}

	void OnGeneratorDestroyedCallback(TheGameObject generator, TheGameObject killer)
	{
		RemoveFromEntitiesToDestroy(generator);
	}

	void SetCurrMissionObj(string set)
	{
		if(mission_state_background_go != null)
			mission_state_background_go.SetActive(true);

		if(mission_state_text != null)
			mission_state_text.Text = set;
	}

	void HideCurrMissionObj()
	{
		if(mission_state_background_go != null)
			mission_state_background_go.SetActive(false);
	}

	void SetEnemiesToKill(string set)
	{
		if(enemies_to_kill_background_go != null)
			enemies_to_kill_background_go.SetActive(true);

		if(enemies_to_kill_text != null)
			enemies_to_kill_text.Text = set;
	}

	void HideEnemiesToKill()
	{
		if(enemies_to_kill_background_go != null)
			enemies_to_kill_background_go.SetActive(false);
	}

	void SpawnIntroShip()
	{
		intro_ship = TheResources.LoadPrefab(intro_enemy_ship_prefab);

		if(intro_ship != null)
		{
			AddToEntitiesToDestroy(intro_ship);

			TheTransform intro_ship_trans = intro_ship.GetComponent<TheTransform>();

			if(intro_ship_trans != null)
			{
				if(intro_ship_spawn != null)
				{
					TheTransform spawn_trans = intro_ship_spawn.GetComponent<TheTransform>();

					intro_ship_trans.LocalPosition = spawn_trans.LocalPosition;

					TheScript movement_script = intro_ship.GetScript("GuillemMovement");

					object[] args = {1};
					movement_script.CallFunctionArgs("SetMovementMode", args);

					if(intro_ship_path != null)
					{
						object[] args2 = {intro_ship_path};
	
						movement_script.CallFunctionArgs("SetPathParent", args2);
					}
				}
			}
		}
	}

	void SpawnAmbushShips()
	{
		TheGameObject spawn1 = TheResources.LoadPrefab(enemy_ship_prefab);
		TheGameObject spawn2 = TheResources.LoadPrefab(enemy_ship_prefab);
				
		TheTransform spawner1 = null;
		TheTransform spawner2 = null;

		if(intro_ship_spawn_2 != null)
			spawner1 = intro_ship_spawn_2.GetComponent<TheTransform>();

		if(intro_ship_spawn_3 != null)
			spawner2 = intro_ship_spawn_3.GetComponent<TheTransform>();

		if(spawn1 != null && spawn2 != null && spawner1 != null && spawner2 != null)
		{
			AddToEntitiesToDestroy(spawn1);
			AddToEntitiesToDestroy(spawn2);

			TheTransform trans1 = spawn1.GetComponent<TheTransform>();
			TheTransform trans2 = spawn2.GetComponent<TheTransform>();

			if(trans1 != null)
				trans1.LocalPosition = spawner1.LocalPosition;

			if(trans2 != null)
				trans2.LocalPosition = spawner2.LocalPosition;
		}
	}

	void SpawnMainMissionShips()
	{
		if(ships_to_spawn > 0)
		{
			if(timer_between_spawn.ReadTime() > time_between_spawn)
			{
				TheGameObject spawned = TheResources.LoadPrefab(enemy_ship_prefab);

				if(spawned != null)
				{
					TheVector3 spawn_pos = new TheVector3(0, 0, 0);

					int rand = (int)TheRandom.RandomRange(0, 6);

					if(rand < 1)
					{
						if(main_ship_spawner1_trans != null)
						{
							spawn_pos = main_ship_spawner1_trans.LocalPosition;
						}
					}
					else if (rand < 2)
					{
						if(main_ship_spawner2_trans != null)
						{
							spawn_pos = main_ship_spawner2_trans.LocalPosition;
						}
					}
					else if (rand < 3)
					{
						if(main_ship_spawner3_trans != null)
						{
							spawn_pos = main_ship_spawner3_trans.LocalPosition;
						}
					}
					else if (rand < 4)
					{
						if(main_ship_spawner4_trans != null)
						{
							spawn_pos = main_ship_spawner4_trans.LocalPosition;
						}
					}

					else if (rand < 5)
					{
						if(main_ship_spawner5_trans != null)
						{
							spawn_pos = main_ship_spawner5_trans.LocalPosition;
						}
					}
					else
					{
						if(main_ship_spawner6_trans != null)
						{
							spawn_pos = main_ship_spawner6_trans.LocalPosition;
						}
					}

					TheTransform spawned_trans = spawned.GetComponent<TheTransform>();

					if(spawned_trans != null)
					{
						spawned_trans.LocalPosition = spawn_pos;
					}
				}		
				

				timer_between_spawn.Start();
				--ships_to_spawn;
			}
		}
	}

	void SlaveWarp()
	{
		warp_time.Start();
		warping = true;
	}

	void SpawnNextWave(int ships)
	{
		ships_to_spawn += ships;
	}

	void AddToEntitiesToDestroy(TheGameObject add)
	{
		entities_to_destroy.Add(add);
	}
	
	void RemoveFromEntitiesToDestroy(TheGameObject remove)
	{
		entities_to_destroy.Remove(remove);
	}
	
	int GetEntitiesToDestroyCount()
	{
		return entities_to_destroy.Count;
	}
}