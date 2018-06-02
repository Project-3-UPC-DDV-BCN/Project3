using TheEngine;
using TheEngine.TheConsole; 

public class MarkerTracker 
{

	public TheGameObject marker;
	private TheGameObject ship;

	private TheScript slave_targeting;  

	void Start () 
	{
		marker.SetActive(false); 
		ship = TheGameObject.Self; 

		slave_targeting = TheGameObject.Find("PlayerCam").GetComponent<TheScript>(0);

		if(slave_targeting != null) TheConsole.Log("targeting detected"); 
		if(ship != null) TheConsole.Log("parent detected"); 
	}

	void Update()
	{
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
	void ShowTargetMarker()
	{
		marker.SetActive(true); 
	}

	void HideTargetMarker()
	{
		marker.SetActive(false); 
	}

}