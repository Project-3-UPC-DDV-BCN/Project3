using TheEngine;
using System.Collections.Generic;

public class IntroManager 
{
	public TheGameObject canvas_go;	
	public TheGameObject text_go;

	public TheGameObject audio_emiter;
	
	TheAudioSource audio_source;
	
	TheText text = null;
	//List<LineText> texts = new List<LineText>();	

	TheTimer timer;
	
	/*public class LineText
	{
		public string text;
		public float time = 1;
	}
	*/
	
	int count = 0;
	void Start () 
	{
		text = text_go.GetComponent<TheText>();
		AddText("You are here to show us your ability", 1);
		AddText("in dogfights Boba prove that your services", 1);
		AddText("can help us to save the galaxy.", 1);
		AddText("Destroy every ships you see,", 1);
		AddText("the galaxy could depend of one of your shots.", 1);
		AddText("That's just the beginning, continue with it,", 1);
		AddText("prove your reputation Boba.", 1);
		
		if(audio_emiter != null)
			audio_source = audio_emiter.GetComponent<TheAudioSource>();

		timer.Start();
	
	}
	
	void Update () 
	{
		UpdateText();
	
		/*if(TheInput.GetControllerButton(0,"CONTROLLER_X") == 1)
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

			UpdateText();
			
			count++;
		}*/
	}

	void SetText(string t)
	{
		text.Text = t;
	}

	void AddText(string t, float time)
	{
		/*LineText lt = new LineText();
		lt.text = t;
		lt.time = time;
		texts.Add(lt);
		*/
	}


	void UpdateText()
	{
		/*if(texts.Count > 0)
		{
			canvas_go.SetActive(true);
			SetText(texts[0].text);

			if(timer.ReadTime() > texts[0].time)
			{
				texts.RemoveAt(0);
				timer.Start();
			}
		}
		else
		{
			canvas_go.SetActive(false);
			SetText("");

		}
		*/
	}
}