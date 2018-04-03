using TheEngine;

public class AllianceSpawn 
{
	bool one_time = true;

	void Start () 
	{
		
	}
	
	void Update () 
	{
		if(one_time)
		{
			TheGameObject game_manager = TheGameObject.Find("GameManager");

			if(game_manager != null)
			{
				TheScript script = game_manager.GetComponent<TheScript>();

				if(script != null)
				{
					TheTransform self_trans = TheGameObject.Self.GetComponent<TheTransform>();

					script.SetVector3Field("position_to_spawn", new TheVector3(self_trans.LocalPosition.x, self_trans.LocalPosition.y, self_trans.LocalPosition.z));
					script.CallFunction("AddToAlliance");
				}
			}

			one_time = false;
		}
	}
}