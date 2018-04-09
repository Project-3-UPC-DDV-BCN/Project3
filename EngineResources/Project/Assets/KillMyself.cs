using TheEngine;
using TheApplication; 

public class KillMyself 
{

	private TheScript hp_tracker; 

	void Start () 
	{
		hp_tracker = TheGameObject.Self.GetComponent<TheScript>(0); 
	}
	
	void Update () 
	{
		if(hp_tracker.GetIntField("amount") <= 0)
		{
			LoadScene("VS3 - EndGameScene"); 
		}
	}
}