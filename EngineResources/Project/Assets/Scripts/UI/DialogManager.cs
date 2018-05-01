using TheEngine;
using System.Collections.Generic;
using TheEngine.TheConsole;

public class DialogManager 
{
	public TheGameObject audio_emiter;
	TheAudioSource audio_source = null;
	
	public TheGameObject text_go;
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
	
	void Start () 
	{
	
	}
	
	void Update () 
	{
		UpdateText();
	}

	void FireDialog(string dialog)
	{
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

					update_info = false;
					timer.Start();
				}

				if(timer.ReadTime() > curr_time)
				{
					if(curr_info + 1 < curr_dialog.Count-1)
					{
						++curr_info;
						update_info = true;	
					}
					else
					{
						curr_dialog = null;
						curr_info = 0;
						curr_time = 0;
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