using TheEngine;
using TheEngine.TheConsole;

public class Level1Manager 
{
	private bool enabled = false;	

	TheScript game_manager_script = null;

	TheScript slave1_script = null;

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
			enabled = (bool)game_manager_script.CallFunctionArgs("GetIsLevel1");

			if(enabled)
				TheConsole.Log("Level1 activated! :D");
		}

		TheGameObject slave1 = (TheGameObject)game_manager_script.CallFunctionArgs("GetSlave1");
		if(slave1 != null)
			slave1_script = slave1.GetScript("EntityProperties");
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
}