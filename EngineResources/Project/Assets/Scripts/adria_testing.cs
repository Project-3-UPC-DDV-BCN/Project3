using TheEngine;
using TheEngine.TheConsole;

public class adria_testing {

	void Start () {
		TheConsole.Log(TheQuaternion.Slerp(new TheQuaternion (0.2f, 0.4f, 0.1f, 1f), new TheQuaternion (0.7f, 0.6f, 0.2f, 1f), 1));
	}
	
	void Update () {
		
	}
}