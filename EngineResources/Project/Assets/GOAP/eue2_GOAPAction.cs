using TheEngine;
using TheEngine.TheGOAPAgent;
using TheEngine.TheConsole;

public class eue2_GOAPAction {
	TheGOAPAgent agent = TheGameObject.Self.GetComponent<TheGOAPAgent>();
	void Start () 
	{
		TheConsole.Log("Action Start");
	}
	
	void Update () 
	{
		TheConsole.Log("Action Update");
	}
	
	void OnComplete () 
	{
		TheConsole.Log("Action Complete");
	}
	
	void OnFail () 
	{

	}
}