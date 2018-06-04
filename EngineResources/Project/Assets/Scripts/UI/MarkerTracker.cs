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
			marker_prf.SetActive(false); 

		TheGameObject slave_cam = TheGameObject.Find("PlayerCam");	

		if(slave_cam != null)
		{
			slave_targeting = slave_cam.GetScript("Targeting");
		}

		ship = TheGameObject.Self; 
	}

	void Update()
	{		
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
				if(target == ship)
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

	public void DestroyObject()
	{
		if(marker_prf != null)
			marker_prf.SetActive(false); 
	}

	void ShowTargetMarker()
	{
		if(marker_prf != null)
			marker_prf.SetActive(true); 
	}

	void HideTargetMarker()
	{
		if(marker_prf != null)
			marker_prf.SetActive(false); 
	}

}