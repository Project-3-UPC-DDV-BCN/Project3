using TheEngine;
using TheEngine.TheGOAPAgent;
using TheEngine.TheConsole;

public class eueueu_GOAPAction {

	TheGOAPAgent agent; 

	void Start () 
	{
		agent = TheGameObject.Self.GetComponent<TheGOAPAgent>();
	}
	
	void Update () 
	{
		TheConsole.Log("ieeee");
	}
	
	void OnComplete () 
	{
	
	}
	
	void OnFail () 
	{

	}
}