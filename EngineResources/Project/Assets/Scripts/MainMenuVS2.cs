using TheEngine;
using TheEngine.TheConsole;

public class MainMenuVS2 
{
	public TheGameObject campaign_button_go;
	public TheGameObject settings_button_go;
	public TheGameObject exit_button_go;
	public TheGameObject explanation_text_go;
	public TheGameObject loading_text_go;

	TheRectTransform campaign_rect = null;
	TheRectTransform settings_rect = null;
	TheRectTransform exit_rect = null;
	TheText	explanation_text = null;	

	void Start () 
	{
		if(campaign_button_go != null)
		{
			campaign_rect = campaign_button_go.GetComponent<TheRectTransform>();
		}

		if(settings_button_go != null)
		{
			settings_rect = settings_button_go.GetComponent<TheRectTransform>();
		}

		if(exit_button_go != null)
		{
			exit_rect = exit_button_go.GetComponent<TheRectTransform>();
		}

		if(explanation_text_go != null)
		{
			explanation_text = explanation_text_go.GetComponent<TheText>();
			
			if(explanation_text != null)
				explanation_text.Text = "";
		}

		if(loading_text_go != null)
			loading_text_go.SetActive(false);
	}
	
	void Update ()
	{
		if(campaign_rect != null)
		{
			if(campaign_rect.OnClickUp)
			{
				if(loading_text_go != null)
					loading_text_go.SetActive(true);
				
				TheApplication.LoadScene("VerticalSlice_NoPanelUIGood_guillemdominguez");
				return;
			}

			if(campaign_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "Play the campaign";
			}
		}

		if(settings_rect != null)
		{
			if(settings_rect.OnClickUp)
			{
				TheConsole.Log("Settings");
			}

			if(settings_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "Change the game settings";
			}
		}

		if(exit_rect != null)
		{
			if(exit_rect.OnClickUp)
			{
				TheApplication.Quit();
				return;
			}

			if(exit_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "Exit the game";
			}
		}

		if(exit_rect != null && campaign_rect != null && settings_rect != null)
		{
			if(!campaign_rect.OnMouseOver && !settings_rect.OnMouseOver && !exit_rect.OnMouseOver)
			{
				if(explanation_text != null)
					explanation_text.Text = "";
			}
		}
	}
}