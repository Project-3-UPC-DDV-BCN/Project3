using TheEngine;
using TheEngine.TheConsole;

public class Level1Manager 
{
	private bool enabled = false;	

	TheGameObject self = null;

	TheScript game_manager_script = null;

	TheScript slave1_script = null;

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

	int curr_mission_state = 0;
	// 1 - Ackbar intro
	// 2 - Follow ship and destroy it
	// 3 - Ackbar intro briefing and see ring
	// 4 - Go to ring and kill ships
	// 5 - Kill generators 
	// 6 - Win

	public TheGameObject intro_ship_spawn;
	public TheGameObject intro_ship_path;

	public string intro_enemy_ship_prefab;
	private TheGameObject intro_ship = null;

	public string enemy_ship_prefab;

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
		
		TheGameObject slave1 = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");
		if(slave1 != null)
			slave1_script = slave1.GetScript("EntityProperties");

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

			object[] args1 = {"AckbarIntro"};
			dialog_manager.CallFunctionArgs("NewDialog", args1);

			object[] args2 = {"AckbarIntro", "This is the intro text that", 3.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args2);

			object[] args3 =  {"AckbarIntro", "will pop on the beggining", 3.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args3);

			object[] args4 =  {"AckbarIntro", "as an intro.", 3.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args4);

			object[] args5 = {"AckbarMissionExplain"};
			dialog_manager.CallFunctionArgs("NewDialog", args5);

			object[] args6 = {"AckbarMissionExplain", "Well done!", 3.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args6);

			object[] args7 = {"AckbarMissionExplain", "This mission is still in creation progress", 3.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args7);

			object[] args8 = {"AckbarMissionExplain", "so there is nothing else for now.", 3.5f};
			dialog_manager.CallFunctionArgs("NewDialogLine", args8);
		}

		HideCurrMissionObj();
		HideEnemiesToKill();

		// Start mission
		NextMissionState();
	}
	
	void Update () 
	{
		UpdateMissionState(curr_mission_state);
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
					object[] args =  {"AckbarIntro"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}

				break;
			}
			case 2:
			{
				SetCurrMissionObj("Follow and kill the enemy ship");
				SetEnemiesToKill("Ships to kill: 1");

				SpawnIntroShip();
				
				break;
			}
			case 3:
			{
				if(dialog_manager != null)
				{
					object[] args =  {"AckbarMissionExplain"};
					dialog_manager.CallFunctionArgs("FireDialog", args);
				}

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
				HideEnemiesToKill();
				HideCurrMissionObj();

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
			TheTransform trans = intro_ship.GetComponent<TheTransform>();

			if(trans != null)
			{
				if(intro_ship_spawn != null)
				{
					TheTransform spawn_trans = intro_ship_spawn.GetComponent<TheTransform>();

					trans.LocalPosition = spawn_trans.LocalPosition;

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
}