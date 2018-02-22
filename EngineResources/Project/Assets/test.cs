using TheEngine;
using TheEngine.TheConsole;

public class test {
	void Start () {
		TheConsole.Log(TheAudio.IsMuted());
	
	}
	
	void Update () {
		TheAudio.SetVolume(100);
	}
}