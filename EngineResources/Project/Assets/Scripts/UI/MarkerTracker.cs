using TheEngine;
using TheEngine.TheConsole; 

public class MarkerTracker 
{

	public TheGameObject marker_prf;

	private TheGameObject marker; 
	private TheGameObject ship;

	private TheScript slave_targeting;  

	void Start () 
	{

		marker = TheGameObject.Duplicate(marker_prf); 
		ship = TheGameObject.Self; 
		FollowShip();

		marker.SetActive(false); 
		
		slave_targeting = TheGameObject.Find("PlayerCam").GetComponent<TheScript>(0);

		if(slave_targeting != null) TheConsole.Log("targeting detected"); 
		if(ship != null) TheConsole.Log("parent detected"); 
	}

	void Update()
	{

		FollowShip();

		if(slave_targeting == null) TheConsole.Log("targeting not detected"); 
		if(ship == null) TheConsole.Log("parent not detected");
 
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

	void FollowShip()
	{
		TheTransform marker_trans = marker.GetComponent<TheTransform>(); 
		TheTransform ship_trans = ship.GetComponent<TheTransform>(); 

		marker_trans.GlobalPosition = ship_trans.GlobalPosition; 
	}
	

	void ShowTargetMarker()
	{
		marker.SetActive(true); 
	}

	void HideTargetMarker()
	{
		marker.SetActive(false); 
	}

}