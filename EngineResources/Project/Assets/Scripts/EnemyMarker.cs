using TheEngine; 
using TheEngine.TheConsole; 
using System.Collections.Generic; 

public class EnemyMarker 
{

	public int range; 

	TheScript game_manager_scp; 

	List<TheGameObject> ships_in_scene;
	List<TheGameObject> ships_with_marker;
	List<TheGameObject> markers; 

	void Start () 
	{
		game_manager_scp = TheGameObject.Self.GetComponent<TheScript>(0); 
		ships_in_scene = (List<TheGameObject>)game_manager_scp.CallFunctionArgs("GetSlaveEnemies");

		ships_with_marker = new List<TheGameObject>();  	
		markers = new List<TheGameObject>(); 	
	}

	void UpdateShipList()
	{
		ships_in_scene = (List<TheGameObject>)game_manager_scp.CallFunctionArgs("GetSlaveEnemies");
	}
	
	void Update () 
	{
		for(int i = 0; i < ships_in_scene.Count ;i++)
		{
			TheVector3 ship_pos = ships_in_scene[i].GetComponent<TheTransform>().GlobalPosition; 

			if(TheCamera.IsObjectInside(ship_pos) == true)
			{		
				TheConsole.Log("OneSeen !!!"); 
				MoveToSeen(i); 
			}
		}	

		for(int i = 0; i < ships_with_marker.Count ;i++)
		{
			TheTransform ship_transform = ships_with_marker[i].GetComponent<TheTransform>(); 
			TheConsole.Log("SeenList not empty !!!"); 

			//Match marker pos
			TheRectTransform curr_marker_rect = markers[i].GetComponent<TheRectTransform>(); 

			if(curr_marker_rect != null)
				curr_marker_rect.Position = TheCamera.WorldPosToCameraPos(ship_transform.GlobalPosition); 

			//Check if it exits the screen 
			if(TheCamera.IsObjectInside(ship_transform.GlobalPosition) == false)
			{		
				MoveToUnseen(i); 
			}		

		} 	
	}

	void MoveToSeen(int pos_in_list)
	{
		for(int i = 0; i < ships_in_scene.Count ;i++)
		{
			if(i == pos_in_list)
			{
				//Add to seen list
				ships_with_marker.Add(ships_in_scene[i]); 
				TheTransform ship_trans = ships_in_scene[i].GetComponent<TheTransform>(); 

				//Load Marker
				TheGameObject marker = TheResources.LoadPrefab("ShipMarker");
				marker.SetParent(TheGameObject.Find("SceneCanvas")); 
				marker.GetComponent<TheTransform>().GlobalPosition = TheCamera.WorldPosToCameraPos(ship_trans.GlobalPosition); 

				TheGameObject.Duplicate(marker); 
				markers.Add(marker); 

				//Delete from global list 
				ships_in_scene.RemoveAt(i); 						
			}				
		}
	}
	
	void MoveToUnseen(int pos_in_list)
	{
		for(int i = 0; i < ships_with_marker.Count ;i++)
		{
			if(i == pos_in_list)
			{
				//Add to unseen list
				ships_in_scene.Add(ships_with_marker[i]); 

				//UnLoad Marker
				//TheGameObject.Destroy(markers[i]); 	

				//Delete from seen & marker list 
				ships_with_marker.RemoveAt(i); 
				markers.RemoveAt(i); 
			}			
		}
	}
}