using TheEngine;
using TheEngine.TheConsole; 

public class MarkerTracker 
{

	private bool is_visible;
	private bool prev_visible;
	private bool just_entered = false; 

	private TheTransform transform;  

	public TheGameObject marker_prf;
	private TheGameObject marker_go;
	private TheRectTransform marker_rect;  

	void Start () 
	{
		transform = TheGameObject.Self.GetComponent<TheTransform>();

		is_visible = false; 
		prev_visible = is_visible; 
		just_entered = false; 
	}
	
	void Update () 
	{
	
		is_visible = TheCamera.IsObjectInside(TheGameObject.Self);

		if(prev_visible != is_visible)
		{
			if(is_visible == true)
				just_entered = true; 
			else
				just_entered = false; 

			prev_visible = is_visible;
		}
			
		if(just_entered)
		{
			//TheGameObject test = TheResources.LoadPrefab("ShipMarker"); 		
			//TheConsole.Log("Prefab loaded");
			
			/*marker_go = TheGameObject.Duplicate(marker_prf);
			TheConsole.Log("Prefab duplicated");
			marker_go.SetParent(TheGameObject.Find("SceneCanvas"));
			marker_rect = marker_go.GetComponent<TheRectTransform>(); 
			marker_rect.Position = new TheVector3(0,0,0); //TheCamera.WorldPosToCameraPos(transform.GlobalPosition); */
		}

		//if(is_visible)
			//marker_rect.Position = new TheVector3(0,0,0); 
			
	}

}