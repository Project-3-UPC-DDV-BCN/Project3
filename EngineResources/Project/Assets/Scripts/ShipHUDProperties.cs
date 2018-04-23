using TheEngine;
using TheEngine.TheConsole;

public class ShipHUDProperties {
	public TheGameObject parent_GO = null;
	private TheScript game_manager = null;
	private TheGameObject GM= null;
	private TheGameObject slave_GO = null;
	public TheGameObject billboard_GO = null;
	private TheGameObject hp_GO;
	private TheGameObject marker_GO;
	
	public float min_scale = 0.1f;
	public float max_scale = 3f;
	
	public float min_distance = 10;
	public float max_distance = 1000;

	void Start () {
		
		marker_GO = TheGameObject.Self.GetChild(0);
		hp_GO = TheGameObject.Self.GetChild(1);
		TheGameObject[] aux = TheGameObject.GetGameObjectsWithTag("GameManager");
		GM = aux[0];
		game_manager = GM.GetScript("GameManager");
		slave_GO = (TheGameObject)game_manager.CallFunctionArgs("GetSlave1");
	
	}
	
	void Update () {
		//Update HP
		if(parent_GO != null)
		{
		//Update HP
			float hp_value;
			hp_value = parent_GO.GetComponent<TheScript>(0).GetFloatField("life");
			TheConsole.Log(hp_value);
			hp_GO.GetComponent<TheProgressBar>().PercentageProgress = hp_value;
		//Update Size
			float distance = TheVector3.Distance(TheGameObject.Self.GetComponent<TheTransform>().GlobalPosition,slave_GO.GetComponent<TheTransform>().GlobalPosition);
			/*
			if(distance <= min_distance)
			{
				//scale buillboard to the minimum
			}else if(distance >= max_distance){
					//scale billboardto the maximum
			}
			else{
				//interpolate between themaximum and the minimum
			}*/

		}
		
	}
}