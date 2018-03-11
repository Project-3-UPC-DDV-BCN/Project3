using TheEngine;
using TheEngine.TheConsole;

public class laser {

	TheRigidBody rb;
	TheTransform trans;
	TheScript script;
	TheScript script2;
	
	//public TheGameObject Test;
	//public TheGameObject Test2;
	//public TheVector3 test3;
	//public string test_s;
	
	TheFactory laser_factory;
    public TheGameObject laser_spawner;

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
	
	void Update () {
		//TheGameObject[] gos = TheGameObject.GetSceneGameObjects();
		//foreach(TheGameObject go in gos)
		//{
		//	TheConsole.Log(go.name);
		//}
		//TheVector3 vec = TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection*2000*TheTime.DeltaTime;
		//TheGameObject.Self.GetComponent<TheRigidBody>().SetLinearVelocity(vec.x, vec.y, vec.z);
		//TheConsole.Log("Laser " + TheGameObject.Self.GetComponent<TheTransform>().ForwardDirection);
		//TheConsole.Log(trans);
		//TheConsole.Log(vec);
	}
	
	void TestMethod()
	{
		//TheConsole.Log(test_s);
	}
}