using TheEngine;

public class TrainingModeManager 
{
	public TheGameObject ship_prefab;

	TheScript game_manager_script = null;

	void Init()
	{
		TheGameObject game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");
	}

	void Start () 
	{
		
	}
	
	void Update () 
	{
		
	}
}