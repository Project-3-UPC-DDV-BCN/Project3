using TheEngine;
using TheEngine.TheConsole;

public class laser 
{
	private TheScript game_manager_scpt; 
	string team; 
	
	TheFactory laser_factory;
    public TheGameObject laser_spawner;
	
	
	void Start ()
	{
		team =  TheGameObject.Find("Slave1").tag; 
		game_manager_scpt = TheGameObject.Find("GameManager").GetComponent<TheScript>(0); 
	//public TheGameObject Test;
	//public TheGameObject Test2;
	//public TheVector3 test3;
	//public string test_s;
	}


	void Start () {
		laser_factory = TheGameObject.Self.GetComponent<TheFactory>();

        laser_factory.StartFactory();
		//TheConsole.Log(TheQuaternion.Slerp(new TheQuaternion(19,4,2,0), new TheQuaternion(1,2,3, 0), 1));
		//rb = TheGameObject.Self.GetComponent<TheRigidBody>();
		//trans = TheGameObject.Self.GetComponent<TheTransform>();
		//TheGameObject[] gos = TheGameObject.GetSceneGameObjects();
		//foreach(TheGameObject go in gos)
		//{
		//	TheConsole.Log(go.name);
		//}
		//TheConsole.Log(TheRandom.RandomInt());
		//TheConsole.Log(TheRandom.RandomFloat());
		//TheConsole.Log(TheRandom.RandomRange(5,20));
		//TheConsole.Log(TheGameObject.Find("Main Camera").name);
		//Slave1Movement slave = TheGameObject.Self.GetComponent<Slave1Movement>();
		//test_s = TheGameObject.Self.tag;
		//script = TheGameObject.Self.GetComponent<TheScript>(0);
		//script2 = Test2.GetComponent<TheScript>(0);
		//script.SetGameObjectField("Test", TheGameObject.Find("Main Camera"));
		//script2.SetGameObjectField("Test", TheGameObject.Find("Main Camera"));
		//script.SetVector3Field("test3",new TheVector3(9,1,2));
		//script2.SetVector3Field("test3",new TheVector3(19,91,22));
		//script2.CallFunction("TestMethod");
		//TheGameObject go = TheGameObject.Self.GetParent();
		//TheConsole.Log(go.name);
		//TheGameObject go2 = TheGameObject.Find("Main Camera").GetParent();
		//TheConsole.Log(go2.name);
		//TheConsole.Log(script.GetGameObjectField("Test"));
		//TheConsole.Log(script2.GetGameObjectField("Test"));
	}
	
	void Update () 
	{
		
	}
	
	void TestMethod()
	{
	
		
	}

	void OnCollisionEnter(TheGameObject other_ship)
	{
		
		string enemy_tag = "TIEFIGHTER"; 

		if(team == "Alliance") 
		{
			if(enemy_tag == "TIEFIGHTER" || enemy_tag == "LANDINGCRAFT")
			{
				game_manager_scpt.SetIntField("score_to_inc", 20);
				game_manager_scpt.CallFunction("AddToScore"); 
				game_manager_scpt.SetIntField("score_to_inc", 0);
			}
			else
			{
				game_manager_scpt.SetIntField("score_to_inc", 10);
				game_manager_scpt.CallFunction("SubstractToScore"); 
				game_manager_scpt.SetIntField("score_to_inc", 0);
			}				
		} 
	}
}