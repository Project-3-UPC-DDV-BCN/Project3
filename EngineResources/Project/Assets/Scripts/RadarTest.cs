using TheEngine;

public class RadarTest 
{

	public TheGameObject radar_go;
	public TheGameObject go_to_add;

	TheRadar radar;
	void Start () 
	{
		radar = radar_go.GetComponent<TheRadar>();
		radar.AddEntity(go_to_add);
		radar.SetMarkerToEntity(go_to_add, "marker_1");
	}
	
	void Update ()
	{
		radar.AddEntity(go_to_add);
	}
}