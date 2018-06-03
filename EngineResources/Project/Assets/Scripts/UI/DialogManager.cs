using TheEngine;
using System.Collections.Generic;
using TheEngine.TheConsole;

public class DialogManager 
{
	TheAudioSource audio_source = null;
	
	TheText text = null;

	TheGameObject canvas_go = null;
	
	Dictionary<string, List<int>> dialogs = new Dictionary<string, List<int>>();

	List<string> texts = new List<string>();
	List<float> times = new List<float>();	
	List<string> audios = new List<string>();	

	TheTimer timer = new TheTimer();

	List<int> curr_dialog = null;
	int curr_info = -1;
	float curr_time = 0.0f;

	bool update_info = false;
	
	void Start()
	{
		audio_source = TheGameObject.Self.GetComponent<TheAudioSource>(1);
	}
	
	void Update() 
	{
		UpdateText();
	}

	void SetTextComponent(TheText txt)
	{
		//TheConsole.Log("Setting text cmp");

		text = txt;
	}

	void SetCanvas(TheGameObject cv)
	{
		//TheConsole.Log("Setting canvas");

		canvas_go = cv;
	}

	void FireDialog(string dialog)
	{
		//TheConsole.Log("Firing dialog");

		curr_dialog = GetDialogList(dialog);

		if(curr_dialog != null && curr_dialog.Count > 0)
		{
			curr_info = 0;

			timer.Start();

			update_info = true;
		}
	}

	bool DialogIsRunning()
	{
		if(curr_dialog == null)
			return false;
		return true;
	}

	void NewDialog(string dialog)
	{
		if(GetDialogList(dialog) == null)
		{
			dialogs.Add(dialog, new List<int>());
		}
	}

	void NewDialogLine(string dialog, string text, float time, string audio)
	{
		List<int> dialog_list = GetDialogList(dialog);

		if(dialog_list != null)
		{
			texts.Add(text);
			times.Add(time);
			audios.Add(audio);

			dialog_list.Add(texts.Count - 1);
		}
	}

	void NewDialogLine(string dialog, string text, float time)
	{
		NewDialogLine(dialog, text, time, "");
	}

	List<int> GetDialogList(string dialog)
	{
		List<int> ret = null;

		if(!dialogs.TryGetValue(dialog, out ret))
			ret = null;

		return ret;
	}

	void UpdateText()
	{
		if(curr_dialog != null)
		{
			if(curr_dialog.Count > 0)
			{
				if(canvas_go != null)
					canvas_go.SetActive(true);
			
				if(update_info)
				{
					int info_index = curr_dialog[curr_info];
	
					string text = texts[info_index];
					float time = times[info_index];
					string audio = audios[info_index];

					SetText(text);
					curr_time = time;
					if(audio_source != null)
						audio_source.Play(audio);

					timer.Start();

					update_info = false;
				
					//TheConsole.Log(text);

				}

				if(timer.ReadTime() > curr_time)
				{
					if(curr_info + 1 <= curr_dialog.Count-1)
					{
						++curr_info;
						update_info = true;	
					}
					else
					{
						curr_dialog = null;
						curr_info = 0;
						curr_time = 0;

						if(canvas_go != null)
							canvas_go.SetActive(false);
					}
				}	
			}
		}
	}

	void SetText(string t)
	{		
		if(text != null)
			text.Text = t;
	}
}