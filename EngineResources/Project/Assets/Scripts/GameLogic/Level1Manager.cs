using TheEngine;
using TheEngine.TheConsole;

public class Level1Manager 
{
	private bool enabled = false;	

	TheScript game_manager_script = null;

	TheScript slave1_script = null;

	public TheGameObject mission_state_text_go;
	TheRectTransform mission_state_text = null;
	public TheGameObject ackbar_canvas_go = null;
	public TheGameObject ackbar_text_go = null;
	TheRectTransform ackbar_text = null;

	TheAudioSource audio_source = null;

	int curr_mission_state = 0;
	// 1 - Ackbar intro
	// 2 - Follow ship and destroy it
	// 3 - Ackbar intro briefing and see ring
	// 4 - Go to ring and kill ships
	// 5 - Kill generators 
	// 6 - Win

	void Init()
	{
		TheGameObject game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");

		if(mission_state_text_go != null)
			mission_state_text = mission_state_text_go.GetComponent<TheRectTransform>();

		if(ackbar_canvas_go != null)
			ackbar_canvas_go.SetActive(false);

		if(ackbar_text_go != null)
			ackbar_text = ackbar_text_go.GetComponent<TheRectTransform>();

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
				if(ackbar_canvas_go != null)
					ackbar_canvas_go.SetActive(true);

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
		
	}

	void OnTurretDestroyedCallback(TheGameObject ship, TheGameObject killer)
	{

	}

	void OnGeneratorDestroyedCallback(TheGameObject ship, TheGameObject killer)
	{

	}
}