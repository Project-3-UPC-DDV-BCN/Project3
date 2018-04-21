using TheEngine;
using TheEngine.TheConsole;

public class TrainingModeManager 
{
	public TheGameObject ship_prefab;
	
	TheScript game_manager_script = null;

	bool enabled = false;	

	int curr_wave = 0;

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
		}
	}
	
	void Update () 
	{
		
	}
}