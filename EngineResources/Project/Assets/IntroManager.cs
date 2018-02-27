using TheEngine;
using System.Collections.Generic;

public class IntroManager 
{
	public TheGameObject canvas_go;	
	public TheGameObject text_go;
	TheText text = null;
	List<string> texts = new List<string>();	

	void Start () 
	{
		text = text_go.GetComponent<TheText>();
		AddText("You are here to show us your well-known");
		AddText("reputation as mercenary.");
		AddText("Destroy the X Wings that are in front");
		AddText("of you, and be careful with the asteroids");
		AddText("We don't want unskilled pilots in");
		AddText("our forces.");
		NextText();
	
	}
	
	void Update () 
	{
		if(TheInput.GetControllerButton(0,"CONTROLLER_X") == 1)
		{
			NextText();
		}
	}

	void SetText(string t)
	{
		text.Text = t;
	}

	void AddText(string t)
	{
		texts.Add(t);
	}

	void NextText()
	{
		if(texts.Count > 0)
		{
			canvas_go.SetActive(true);
			SetText(texts[0]);
			texts.RemoveAt(0);
		}
		else
		{
			canvas_go.SetActive(false);
			SetText("");

		}
	}
}