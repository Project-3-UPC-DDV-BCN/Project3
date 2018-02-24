using TheEngine;

public class progressbar
 {

	public TheProgressBar bar = null;

	void Start () 
	{
		bar = TheGameObject.Self.GetComponent<TheProgressBar>();
		bar.PercentageProgress = 100.0f;
	}
	
	void Update () {
		
	}
}