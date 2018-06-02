using TheEngine;

public class HitMarker 
{

	public TheGameObject hit_marker; 
	public int show_time; 

	private float timer; 
	private bool showing_marker; 

	void Start () 
	{
		hit_marker.SetActive(false);
		timer = 0; 
	}
	
	void Update () 
	{
		if(showing_marker)
		{
			timer++; 
			if(timer >= show_time)
			{
				timer = 0;
				showing_marker = false; 
				hit_marker.SetActive(false); 
			}
		}
	}

	public void ShowHitMarker()
	{
		hit_marker.SetActive(true);
		showing_marker = true; 
	}
}