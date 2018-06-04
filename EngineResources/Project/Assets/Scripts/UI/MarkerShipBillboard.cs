using TheEngine;
using TheEngine.TheConsole; 

public class MarkerShipBillboard 
{

	public TheGameObject ship_marker;
	private TheGameObject target_mark; 

	private TheScript slave_targeting;
    private TheScript properties;
	private TheGameObject ship;  

	private bool target_active; 

	void Start () 
	{
		if(ship_marker != null)
			target_mark = TheGameObject.Duplicate(ship_marker);
	
		if(target_mark != null)
			target_mark.SetActive(false); 

		TheGameObject slave_cam = TheGameObject.Find("PlayerCam");	
		properties = TheGameObject.Self.GetScript("EntityProperties");


		if(slave_cam != null)
		{
			slave_targeting = slave_cam.GetScript("Targeting"); // Aixi ja no es pillen scripts
		}

		ship = TheGameObject.Self;
		target_active = false;  
	}

	public void DeleteObject()
	{
		TheGameObject.Destroy(target_mark); 
	}

	void FollowShip()
	{
		TheTransform marker_trans = target_mark.GetComponent<TheTransform>(); 
		marker_trans.GlobalPosition = ship.GetComponent<TheTransform>().GlobalPosition;
	}
	
	void Update () 
	{

		if(target_active == true)
			FollowShip(); 

		if(slave_targeting == null) 
			TheConsole.Log("targeting not detected"); 

		if(ship == null) 
			TheConsole.Log("parent not detected");
 
		if(slave_targeting != null)
		{
			TheGameObject target = (TheGameObject)slave_targeting.CallFunctionArgs("GetTarget"); 	

			if(target != null)
			{
				TheConsole.Log("target returned"); 
				if(target.GetComponent<TheTransform>() == ship.GetComponent<TheTransform>())
				{
					ShowTargetMarker();
					target_active = true; 
				}
				else
				{
					HideTargetMarker(); 
					target_active = false; 
				}
			}	
		}			
		
	}


	void ShowTargetMarker()
	{
		target_mark.SetActive(true); 
	}

	void HideTargetMarker()
	{
		target_mark.SetActive(false); 
	}
}