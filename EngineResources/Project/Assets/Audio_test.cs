using TheEngine;

public class Audio_test {

	public TheGameObject audio; 
	public TheAudioSource audio_source;
	private string audio_dub1 = "Play_Dialogue1";
	private string audio_dub2 = "Play_Dialogue2";
	private string audio_dub3 = "Play_Dialogue3";
	private string audio_dub4 = "Play_Dialogue4";

	void Start () {
		audio_source = audio.GetComponent<TheAudioSource>();
		audio_source.Play("Play_Engine");
		audio_source.SetMyRTPCvalue("asdas",5);
	}
	
	void Update () {
		if (TheInput.IsKeyDown("1"))
		{
			audio_source.Play(audio_dub1);
		}
		if (TheInput.IsKeyDown("2"))
		{
			audio_source.Play(audio_dub2);
		}
		if (TheInput.IsKeyDown("3"))
		{
			audio_source.Play(audio_dub3);
		}
		if (TheInput.IsKeyDown("4"))
		{
			audio_source.Play(audio_dub4);
		}
	}
}