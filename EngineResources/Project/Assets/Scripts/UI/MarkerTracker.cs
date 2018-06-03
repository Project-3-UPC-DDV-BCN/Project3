using TheEngine;
using TheEngine.TheConsole; 

public class MarkerTracker 
{
	public TheGameObject marker_prf;

	private TheGameObject marker = null; 
	private TheGameObject ship = null;

	private TheScript slave_targeting = null;  

	void Start () 
	{
		if(marker_prf != null)
		{
			marker = TheGameObject.Duplicate(marker_prf); 

			if(marker != null)
			{
				ship = TheGameObject.Self; 
				FollowShip();

				marker.SetActive(false); 
		
				TheGameObject slave_cam = TheGameObject.Find("PlayerCam");	

				if(slave_cam != null)
				{
					slave_targeting = slave_cam.GetScript("Targeting"); // Aixi ja no es pillen scripts
				}

				if(slave_targeting != null) TheConsole.Log("targeting detected"); 
				if(ship != null) TheConsole.Log("parent detected"); 
			}
		}
	}

	void Update()
	{
		
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
				if(target.GetComponent<TheTransform>() == ship.GetComponent<TheTransform>())
				{
					ShowTargetMarker();
				}
				else
				{
					HideTargetMarker(); 
				}
			}	
		}
	}	

	void FollowShip()
	{
		if(marker != null && ship != null)
		{
			TheTransform marker_trans = marker.GetComponent<TheTransform>(); 
			TheTransform ship_trans = ship.GetComponent<TheTransform>(); 

			if(marker_trans != null && ship_trans != null)
				marker_trans.GlobalPosition = ship_trans.GlobalPosition; 
		}
	}
	

	void ShowTargetMarker()
	{
		if(marker != null)
			marker.SetActive(true); 
	}

	void HideTargetMarker()
	{
		if(marker != null)
			marker.SetActive(false); 
	}

}