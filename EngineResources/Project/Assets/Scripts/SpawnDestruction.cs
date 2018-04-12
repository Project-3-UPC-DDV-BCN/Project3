using TheEngine;

public class SpawnDestruction {

    private string ship_tag;
    private TheVector3 ship_pos;

    private TheGameObject ship_to_spaw;
	private TheScript ship_properties; 

	private bool model_changed; 

	void Start ()
    {
        ship_tag = TheGameObject.Self.tag;
        ship_pos = TheGameObject.Self.GetComponent<TheTransform>().GlobalPosition;
		ship_properties = TheGameObject.Self.GetComponent<TheScript>(0);
		model_changed = false; 
            
	}

	void Update()
	{

		if(ship_properties.GetIntField("hp") <= 0 && model_changed == false)
		{
			SwapModels(); 
			model_changed = true; 
		}

	}

	void SwapModels()
	{	
		TheGameObject.Self.SetActive(false); 

        if (ship_tag == "TIEFIGHTER")
		{
			TheGameObject.Self.SetActive(false); 
            ship_to_spaw = TheResources.LoadPrefab("TieFighterDestruct"); 
			TheGameObject go = TheGameObject.Duplicate(ship_to_spaw); 
			go.GetComponent<TheTransform>().GlobalPosition = ship_pos;
		}
	}
	
}