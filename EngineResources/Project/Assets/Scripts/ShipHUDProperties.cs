using TheEngine;
//using TheEngine.TheConsole; 

public class ShipHUDProperties {
	public TheGameObject parent_GO = null;
	private TheScript game_manager = null;
	private TheGameObject GM= null;
	private TheGameObject slave_GO = null;
	public TheGameObject billboard_GO = null;
	private TheGameObject hp_GO;
	private TheGameObject marker_GO;
	
	
	TheGameObject self = null;

	void Start () {
		self = TheGameObject.Self;
		marker_GO = self.GetChild(0);
		hp_GO = self.GetChild(1);
		GM = TheGameObject.Find("GameManager");
		if(GM != null)
		{
			game_manager = GM.GetScript("GameManager");
			slave_GO = (TheGameObject)game_manager.CallFunctionArgs("GetSlave1");
		}
	
	}
	
	void Update () {
		//Update HP
		if(parent_GO != null)
		{
		//Update HP
			float hp_value;
			hp_value = parent_GO.GetComponent<TheScript>(0).GetFloatField("life");
			//TheConsole.Log(hp_value);
			hp_GO.GetComponent<TheProgressBar>().PercentageProgress = hp_value;
		}
		
	}
}