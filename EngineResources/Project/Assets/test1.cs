using TheEngine;
using TheEngine.TheConsole;

public class test1 {

	private TheScript game_manager_scpt; 
	string team; 

	private TheScript ship_hit_scpt; 

	void Start () {
		game_manager_scpt = TheGameObject.Find("GameManager").GetComponent<TheScript>(0);
		game_manager_scpt.CallFunction("AddToScore");
		ship_hit_scpt = TheGameObject.Find("test").GetComponent<TheScript>();
		ship_hit_scpt.CallFunction("SubstractHP"); 
	}
	
	void Update () {
		
	}
	
	void Test()
	{
		TheConsole.Log("Test");
	}
	
	void Test2()
	{
		TheConsole.Log("Test2");
	}
}