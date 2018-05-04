using TheEngine;

public class Triggerer 
{
	public string triggerer_name;

	TheScript game_manager_script = null;

	void Init()
	{
		TheGameObject game_manager = TheGameObject.Find("GameManager");
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");
	}

	void OnTriggerEnter(TheCollisionData coll)
	{
		//TheConsole.Log("Laser trigger 0");

		if(coll == null)
			return;

		TheGameObject other_go = coll.Collider.GetGameObject();

		if(other_go == null)
			return;

		if(game_manager_script != null)
		{
			object[] args = {triggerer_name, other_go};
			game_manager_script.CallFunctionArgs("CallTrigger", args);
		}
	}
}