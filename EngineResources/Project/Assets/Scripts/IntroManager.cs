using TheEngine;
using System.Collections.Generic;
using TheEngine.TheConsole;

public class IntroManager 
{
	public TheGameObject canvas_go;	
	public TheGameObject text_go;

	public TheGameObject audio_emiter;
	
	TheAudioSource audio_source;
	
	TheText text = null;
	bool update_line = true;
	string current_audio = "";
	List<string> texts = new List<string>();
	List<float> times = new List<float>();	
	List<string> audios = new List<string>();	

	TheTimer timer = new TheTimer();
	
	void Start () 
	{
		text = text_go.GetComponent<TheText>();
		AddTextLine("You are here to show us your ability", 1.7f, "Play_Ackbar_dialogue_01");
		AddTextLine("in dogfights Boba prove that your services", 2);
		AddTextLine("can help us to save the galaxy.", 2);
		AddTextLine("Destroy every ships you see,", 1.5f, "Play_Ackbar_dialogue_02");
		AddTextLine("the galaxy could depend of one of your shots.", 2.4f);
		AddTextLine("That's just the beginning, continue with it,", 2, "Play_Ackbar_dialogue_03");
		AddTextLine("prove your reputation Boba.", 2.5f);
		
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

	void AddTextLine(string t, float time)
	{
		texts.Add(t);
		times.Add(time);
		audios.Add("");
	}
	
	void AddTextLine(string t, float time, string audio_name)
	{
		texts.Add(t);
		times.Add(time);
		audios.Add(audio_name);
	}

	void UpdateText()
	{
		if(texts.Count > 0)
		{
			canvas_go.SetActive(true);

			if(update_line)
			{
				if(texts.Count > 0)
				{
					SetText(texts[0]);
					if(audios[0] != "")
					{
						if(audio_source != null)	
						{
							audio_source.Stop(current_audio);
							current_audio = audios[0];
							audio_source.Play(current_audio);
						}		
					}

					timer.Start();
				}	
				
				update_line = false;
			}
		

			if(timer.ReadTime() > times[0])
			{
				texts.RemoveAt(0);
				times.RemoveAt(0);
				audios.RemoveAt(0);

				update_line = true;
			}

		}
		else
		{
			canvas_go.SetActive(false);
			SetText("");

		}
	}
}