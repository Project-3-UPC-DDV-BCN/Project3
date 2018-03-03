using TheEngine;
using TheEngine.TheGOAPAgent;
using TheEngine.TheConsole;

public class eue2_GOAPAction {

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