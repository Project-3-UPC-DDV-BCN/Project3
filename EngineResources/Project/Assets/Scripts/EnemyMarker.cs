using TheEngine; 
using TheEngine.TheConsole; 
using System.Collections.Generic; 

public class EnemyMarker 
{

	public int range; 

	TheScript game_manager_scp; 

	List<TheGameObject> ships_in_scene;
	List<TheGameObject> ships_with_marker; 

	void Start () 
	{
		range = 0;
		game_manager_scp = TheGameObject.Self.GetComponent<TheScript>(0); 
		ships_in_scene = (List<TheGameObject>)game_manager_scp.CallFunctionArgs("GetSlaveEnemies"); 		
	}
	
	void Update () 
	{
		for(int i = 0; i < ships_in_scene.Count ;i++)
		{
			TheVector3 ship_pos = ships_in_scene[i].GetComponent<TheTransform>().GlobalPosition; 

			if(TheCamera.IsObjectInside(ship_pos) == true)
			{		
				
			}

		}
				
	}
	
	void MoveToSeen()
	{
		
	}
	
	void MoveToUnseen()
	{
		
	}
}