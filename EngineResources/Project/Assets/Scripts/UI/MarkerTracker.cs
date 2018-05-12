using TheEngine;
using TheEngine.TheConsole; 

public class MarkerTracker 
{

	private bool is_visible;
	private bool prev_visible;
	private bool just_entered = false; 

	private TheTransform transform;  

	private TheGameObject marker_go;
	private TheRectTransform marker_rect;  

	void Start () 
	{
		transform = TheGameObject.Self.GetComponent<TheTransform>();
		is_visible = TheCamera.IsObjectInside(transform.GlobalPosition); 
		prev_visible = is_visible; 
		just_entered = false; 
	}
	
	void Update () 
	{
		is_visible = TheCamera.IsObjectInside(transform.GlobalPosition);

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
			marker_go = TheGameObject.Duplicate(TheResources.LoadPrefab("ShipMarker"));
			marker_go.SetParent(TheGameObject.Find("SceneCanvas"));
			marker_rect = marker_go.GetComponent<TheRectTransform>(); 
			marker_rect.Position = TheCamera.WorldPosToCameraPos(transform.GlobalPosition); 
		}

		if(is_visible)
			marker_rect.Position = TheCamera.WorldPosToCameraPos(transform.GlobalPosition); 
			
	}

}