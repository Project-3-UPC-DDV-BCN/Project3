using TheEngine;
using System.Collections.Generic;

public class IntroManager 
{
	public TheGameObject canvas_go;	
	public TheGameObject text_go;

	public TheGameObject audio_emiter;
	
	TheAudioSource audio_source;
	
	TheText text = null;
	List<string> texts = new List<string>();	
	
	int count = 0;
	void Start () 
	{
		text = text_go.GetComponent<TheText>();
		AddText("You are here to show us your ability");
		AddText("in dogfights Boba prove that your services");
		AddText("can help us to save the galaxy.");
		AddText("Destroy every ships you see,");
		AddText("the galaxy could depend of one of your shots.");
		AddText("That's just the beginning, continue with it,");
		AddText("prove your reputation Boba.");
		NextText();

		audio_source = audio_emiter.GetComponent<TheAudioSource>();
		audio_source.Play("Play_Dialogue1");
	
	}
	
	void Update () 
	{
		if(TheInput.GetControllerButton(0,"CONTROLLER_X") == 1)
		{
			if (count == 2)
			{
				audio_source.Stop("Play_Dialogue1");
				audio_source.Play("Play_Dialogue2");
			}
			else if (count == 4)
			{
				audio_source.Stop("Play_Dialogue2");
				audio_source.Play("Play_Dialogue3");
			}

			NextText();
			
			count++;
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