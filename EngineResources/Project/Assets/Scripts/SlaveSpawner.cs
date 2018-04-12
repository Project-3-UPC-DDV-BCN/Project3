using TheEngine;

public class SlaveSpawner {

	private TheGameObject rebels_spawn; 
	private TheGameObject empire_spawn; 
	
	private TheScript game_manager; 

	void Start () 
	{
		rebels_spawn = TheGameObject.Find("RebelsSpawn"); 
		empire_spawn = TheGameObject.Find("New GameObject(2)"); 

		game_manager = TheGameObject.Find("GameManager").GetComponent<TheScript>(0); 

		string team = game_manager.GetStringField("team");

		TheGameObject slave1_root = TheResources.LoadPrefab("Slave1_PrefabPuzzl");
		TheGameObject slave1 = TheGameObject.Duplicate(slave1_root); 

		if(team == "rebels") 
			slave1.GetComponent<TheTransform>().GlobalPosition = rebels_spawn.GetComponent<TheTransform>().GlobalPosition; 
		
		else if (team == "empire") 
			slave1.GetComponent<TheTransform>().GlobalPosition =  rebels_spawn.GetComponent<TheTransform>().GlobalPosition; 			
	}
}