using TheEngine;

public class Level1Manager 
{
	private bool enabled = false;	

	TheScript game_manager_script = null;

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

			enabled = true;
		}
	}
	
	void Update () 
	{
		
	}

	void CheckWinLose()
	{
		
	}

	void OnShipDestroyedCallback(TheGameObject ship)
	{
		
	}
}