using TheEngine;

public class progressbar
{
	public TheText text = null;

	void Start () 
	{
		text = TheGameObject.Self.GetComponent<TheText>();
		text.Text = "hi";
	}
}