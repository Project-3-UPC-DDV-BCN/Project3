using TheEngine;
using TheEngine.TheConsole;

public class HUDProperties {
	public TheGameObject parent_GO;
	private TheGameObject hp_GO;

	void Start () {
		parent_GO = TheGameObject.Self.GetParent();
		hp_GO = TheGameObject.Self.GetChild(1);
	}
	
	void Update () {
		//Update HP
		if(parent_GO != null)
		{
			float hp_value;
			hp_value = parent_GO.GetComponent<TheScript>(0).GetFloatField("life");
			TheConsole.Log(hp_value);
			//hp_GO.GetComponent<TheProgressBar>().SetPercentageProgress(hp_value);
		}	/*
		// MArk Target
		if(parent_GO != null)
		{
			//Change Color?
			//Need to know how are we going to mark the objectives
		}	*/
	}
}