using TheEngine;

public class HitMarker 
{

	public TheGameObject hit_marker; 
	public float show_time; 

	private TheTimer timer; 
	private bool showing_marker; 

	void Start () 
	{
		hit_marker.SetActive(false);
		timer = new TheTimer(); 
	}
	
	void Update () 
	{
		if(showing_marker)
		{ 
			if(timer.ReadTime() >= show_time)
			{
				timer.Stop(); 
				showing_marker = false; 
				hit_marker.SetActive(false); 
			}
		}
	}

	public void ShowHitMarker()
	{
		hit_marker.SetActive(true);
		showing_marker = true;
		timer.Start();  
	}
}