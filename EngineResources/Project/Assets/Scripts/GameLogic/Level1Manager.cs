using TheEngine;
using TheEngine.TheConsole;

public class Level1Manager 
{
	private bool enabled = false;	

	TheGameObject self = null;

	TheScript game_manager_script = null;

	TheGameObject slave1 = null;
	TheTransform slave_trans = null;

	TheScript slave1_script = null;
	TheScript slave1_movement_script = null;
	TheScript slave1_shooting_script = null;

	public TheGameObject mission_state_background_go;
	public TheGameObject mission_state_text_go;
	TheText mission_state_text = null;
	public TheGameObject enemies_to_kill_background_go;
	public TheGameObject enemies_to_kill_text_go;
	TheText enemies_to_kill_text = null;
	public TheGameObject ackbar_canvas_go = null;
	public TheGameObject ackbar_text_go = null;
	TheText ackbar_text = null;

	TheScript dialog_manager = null;

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


	public TheGameObject intro_ship_spawn;
	public TheGameObject intro_ship_path;

	public TheGameObject intro_ship_spawn_2;
	public TheGameObject intro_ship_spawn_3;

	public string intro_enemy_ship_prefab;
	private TheGameObject intro_ship = null;
	private TheTransform intro_ship_trans = null;
	
	public string enemy_ship_prefab;

	private TheTimer attack_intro_ship = new TheTimer();

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

		if(fight_zone != null)
			fight_trans = fight_zone.GetComponent<TheTransform>();

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

			object[] args = {ackbar_canvas_go};
			dialog_manager.CallFunctionArgs("SetCanvas", args);

			object[] args0 = {ackbar_text};
			dialog_manager.CallFunctionArgs("SetTextComponent", args0);

			object[] args1 = {"AckbarIntro1"};
			dialog_manager.CallFunctionArgs("NewDialog", args1);

			object[] args2 = {"AckbarIntro1", "Bobba, you are behind enemy lines.", 6.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args2);

			object[] args3 =  {"AckbarIntro1", "Be careful, try to not get detected", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args3);

			object[] args8 =  {"AckbarIntro1", "or things will heat up in no time!", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args8);

			object[] args9 = {"AckbarIntro2"};
			dialog_manager.CallFunctionArgs("NewDialog", args9);

			object[] args10 = {"AckbarIntro2", "Look, there is an enemy ship at your right!.", 5.0f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args10);

			object[] args5 = {"AckbarIntro3"};
			dialog_manager.CallFunctionArgs("NewDialog", args5);

			object[] args6 = {"AckbarIntro3", "Don't get trigger happy, Bobba.", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args6);

			object[] args7 = {"AckbarIntro3", "Follow that ship. Let's see where this leads to...", 6.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args7);

			object[] args11 = {"AckbarAttack1"};
			dialog_manager.CallFunctionArgs("NewDialog", args11);

			object[] args12 = {"AckbarAttack1", "They can't catch you, Bobba! Don't hesitate!,", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args12);

			object[] args13 = {"AckbarAttack1", "Shoot'em down!", 4.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args13);

		}

		HideCurrMissionObj();
		HideEnemiesToKill();

		// Start mission
		NextMissionState();
	}
	
	void Update () 
	{
		UpdateMissionState(curr_mission_state);
		UpdateAudio();
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
					object[] args =  {"AckbarIntro1"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
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
				if(dialog_manager != null)
				{
					object[] args =  {"AckbarAttack1"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}

				break;
			}
			case 5:
			{
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
				break;
			}
			case 5:
			{
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
				break;
			}
			case 5:
			{
				break;
			}
		}
	}

	void OnShipDestroyedCallback(TheGameObject ship, TheGameObject killer)
	{
		if(curr_mission_state == 2)
		{
			if(ship == intro_ship)
			{
				NextMissionState();
			}
		}
	}

	void OnTurretDestroyedCallback(TheGameObject ship, TheGameObject killer)
	{

	}

	void OnGeneratorDestroyedCallback(TheGameObject ship, TheGameObject killer)
	{

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
			intro_ship_trans = intro_ship.GetComponent<TheTransform>();

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
			TheTransform trans1 = spawn1.GetComponent<TheTransform>();
			TheTransform trans2 = spawn2.GetComponent<TheTransform>();

			if(trans1 != null)
				trans1.LocalPosition = spawner1.LocalPosition;

			if(trans2 != null)
				trans2.LocalPosition = spawner2.LocalPosition;
		}
	}

	void CallTrigger(string trigger_name, TheGameObject go_triggerer)
	{
		if(trigger_name == "IntroTrigger")
		{
			if(go_triggerer == slave1)
			{
				NextMissionState();
			}
		}
	}
}