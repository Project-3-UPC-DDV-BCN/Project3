using TheEngine;

public class Audio_test {

	public TheGameObject audio; 
	public TheAudioSource audio_source;

	void Start () {
		audio_source = audio.GetComponent<TheAudioSource>();
		bool a = audio_source.Play("Play_Engine");
	}
	
	void Update () {
		
	}
}