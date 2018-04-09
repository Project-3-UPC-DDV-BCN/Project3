using TheEngine;

public class KillSlave {

	private TheScript hp_tracker; 

	void Start () {
		hp_tracker = TheGameObject.Self.GetComponent<TheScript>(0); 
	}
	
	void Update () {

		if(hp_tracker.GetIntField("amount") <= 0)
			TheApplication.LoadScene("VS3 - EndGameScene"); 
	}
}