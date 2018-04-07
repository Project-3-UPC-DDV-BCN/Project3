using TheEngine;

public class ByeByeTie {
	
	bool delete_obj; 
	private TheScript ship_props; 

	void Start () 
	{
		delete_obj = false; 
		ship_props = TheGameObject.Self.GetComponent<TheScript>(0); 
	}
	
	void Update () 
	{

		if(ship_props.GetIntField("hp") <= 0) 
			delete_obj = true; 

		if(delete_obj)
			TheGameObject.Self.SetActive(false); 
	}
}