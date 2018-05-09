using TheEngine; 
using TheEngine.TheConsole; 
using System.Collections.Generic; 

public class EnemyMarker 
{

	public int range; 

	TheCamera main_camera;  
	TheScript game_manager_scp; 

	List<TheGameObject> ships_in_scene; 

	void Start () 
	{
		range = 0;
		main_camera =  TheGameObject.Find("PlayerCam").GetComponent<TheCamera>(0); 
		game_manager_scp = TheGameObject.Self.GetComponent<TheScript>(0); 
		ships_in_scene = (List<TheGameObject>)game_manager_scp.CallFunctionArgs("GetSlaveEnemies"); 		
	}
	
	void Update () 
	{
		for(int i = 0; i < ships_in_scene.Count ;i++)
		{
			TheVector3 ship_pos = ships_in_scene[i].GetComponent<TheTransform>().GlobalPosition; 
			TheVector3 screen_position = TheCamera.WorldPosToCameraPos(ship_pos);

			if(screen_position.z < range)
			{		
				TheConsole.Log(screen_position.x);
				TheConsole.Log(screen_position.y);
			}

		}
				
	}
}