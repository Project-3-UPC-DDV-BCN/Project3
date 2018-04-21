using TheEngine;
using TheEngine.TheConsole;
using TheEngine.TheMath;

using System.Collections.Generic;

public class EnemyHUD {

	private List<TheGameObject> enemies  = new List<TheGameObject>();
	private List<TheGameObject> enemies_in_front  = new List<TheGameObject>();
	private TheGameObject self = null;
	private TheVector3 self_pos = new TheVector3(0,0,0);
	private TheVector3 self_front = new TheVector3(0,0,0);
	private TheVector3 self_up = new TheVector3(0,0,0);
	private float min_dist = 0.1f;
	private float max_dist = 100f;
	private bool isVisible = false;

	TheScript GameManager = null;
	TheGameObject gm_GO = null;

	//CANVAS AUXILIARS
	public TheGameObject HUD1 = null;
	public TheGameObject HUD2 = null;
	public TheGameObject HUD3 = null;
	
	
	void Start () {

		self = TheGameObject.Self;

		TheGameObject[] aux = TheGameObject.GetGameObjectsWithTag("GameManager");
		if(aux.Length>0) 
		{
			gm_GO = aux[0];
		}
			


		if(gm_GO == null)
			TheConsole.Log("No game Manager detected");
		else if(gm_GO != null)
		{
			GameManager = gm_GO.GetComponent<TheScript>();
		}
		
		self_pos = self.GetComponent<TheTransform>().GlobalPosition;
		self_front = self.GetComponent<TheTransform>().ForwardDirection;
		self_up = self.GetComponent<TheTransform>().UpDirection;
		enemies = (List<TheGameObject>)GameManager.CallFunctionArgs("GetSlaveEnemies");
	}
	
	void Update () {
		//Step1 GET ENEMIES in front
		int j = 0;
		int aux_HUD_counter= 0;
		enemies_in_front.Clear();
		for(int i = 0; i < enemies.Count;i++)
		{
			isVisible = false;
			
			TheVector3 directionToTarget = enemies[i].GetComponent<TheTransform>().GlobalPosition - self.GetComponent<TheTransform>().GlobalPosition;
       		float angle = TheVector3.AngleBetween(self.GetComponent<TheTransform>().ForwardDirection, directionToTarget);
        	
 
        	if (TheMath.Abs (angle) < 90) {
            	isVisible = true;
       		}else {
           		isVisible = false;
        	}

			if(isVisible){
				enemies_in_front.Add(enemies[i]);
				TheConsole.Log("enemy detected");
				TheConsole.Log(enemies_in_front[j].name);
				j++; 
			}
			else if(!isVisible)
			{
				TheConsole.Log("no enemies detected");
			}
		}
		
		//Step2
			//TO 2D	
	

		//Step3
			//Update UI
		
			aux_HUD_counter = enemies_in_front.Count;
		for(int k = 0; k < enemies_in_front.Count;k++)
		{
			if(HUD1!= null && k == 0){
				TheVector3 new_pos1 = new TheVector3(enemies_in_front[k].GetComponent<TheTransform>().GlobalPosition.x,enemies_in_front[k].GetComponent<TheTransform>().GlobalPosition.y,0);
				HUD1.GetComponent<TheTransform>().GlobalPosition = new_pos1; 
			}
			else if(HUD2 != null && k == 1){
				TheVector3 new_pos2 = new TheVector3(enemies_in_front[k].GetComponent<TheTransform>().GlobalPosition.x,enemies_in_front[k].GetComponent<TheTransform>().GlobalPosition.y,0);
				HUD2.GetComponent<TheTransform>().GlobalPosition = new_pos2; 
			}
			else if(HUD3 != null && k == 2){
				TheVector3 new_pos3 = new TheVector3(enemies_in_front[k].GetComponent<TheTransform>().GlobalPosition.x,enemies_in_front[k].GetComponent<TheTransform>().GlobalPosition.y,0);
				HUD3.GetComponent<TheTransform>().GlobalPosition = new_pos3;
			}
				
		}


	}
}