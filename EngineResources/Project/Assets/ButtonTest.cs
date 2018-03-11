using TheEngine;
using TheEngine.TheConsole;

public class ButtonTest
 {
	public TheGameObject button_go;
	TheRectTransform button;

	void Start () 
	{
		button = button_go.GetComponent<TheRectTransform>();
	}
	
	void Update () 
	{

		if(button.OnClick)
		{
			TheConsole.Warning("Click");
		}
/*
		
		if(button.OnClickUp)
		{
			TheConsole.Warning("ClickUp");
		}
		

		if(button.OnClickDown)
		{
			TheConsole.Warning("ClickDown");
		}

/*
		if(button.OnMouseOver)
		{
			TheConsole.Warning("MouseOver");
		}
*/
/*
		if(button.OnMouseEnter)
		{
			TheConsole.Warning("MouseEnter");
		}
		if(button.OnMouseOut)
		{
			TheConsole.Warning("MouseOut");
		}
*/
	}
}