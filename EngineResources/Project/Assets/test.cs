using TheEngine;
using TheEngine.TheConsole;

public class test {
	TheAudio.SetMute(true);
	void Start () {
		TheConsole.Log(TheAudio.IsMuted());
	}
	
	void Update () {
		
	}
}