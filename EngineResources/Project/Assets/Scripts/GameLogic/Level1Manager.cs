using TheEngine;
using TheEngine.TheConsole;

public class Level1Manager 
{
	private bool enabled = false;	

	TheScript game_manager_script = null;

	TheScript slave1_script = null;

	public TheGameObject mission_state_text_go;
	TheRectTransform mission_state_text = null;
	TheAudioSource audio_source;

	void Init()
	{
		TheGameObject game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");

		if(mission_state_text_go != null)
			mission_state_text = mission_state_text_go.GetComponent<TheRectTransform>();
	}

	void Start () 
	{
		if(game_manager_script != null)
		{
			enabled = (bool)game_manager_script.CallFunctionArgs("GetIsLevel1");

			if(enabled)
				TheConsole.Log("Level1 activated! :D");
		}
		
		audio_source = TheGameObject.Self.GetComponent<TheAudioSource>();
		TheGameObject slave1 = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");
		if(slave1 != null)
			slave1_script = slave1.GetScript("EntityProperties");

		if(audio_source!= null)
			audio_source.Play("Play_Music");
	}
	
	void Update () 
	{
		
	}

	void CheckWinLose()
	{
		
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