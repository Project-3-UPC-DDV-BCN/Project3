using TheEngine;
using TheEngine.TheConsole; 
using TheEngine.TheMath;

using System.Collections.Generic;

public class EnemyHUD {

	private List<TheGameObject> enemies  = new List<TheGameObject>();
	private List<TheGameObject> enemies_in_front  = new List<TheGameObject>();
	private List<TheGameObject> prev_enemies_in_front  = new List<TheGameObject>();

	private TheGameObject self = null;
	private TheVector3 self_pos = new TheVector3(0,0,0);
	private TheVector3 self_front = new TheVector3(0,0,0);
	private TheVector3 self_up = new TheVector3(0,0,0);
	private float min_dist = 0.1f;
	private float max_dist = 100f;
	private bool isVisible = false;

	TheScript GameManager = null;
	TheGameObject gm_GO = null;

    public TheGameObject targeting_go;
    private TheScript targeting_script = null;
	

	//CANVAS GO
	public TheGameObject canvas = null;

	private List<TheGameObject> markers = new List<TheGameObject>();

	TheTransform self_transform = null;
	
	void Start () {

		self = TheGameObject.Self;

		gm_GO = TheGameObject.Find("GameManager");

		if(gm_GO == null)
		{
			//TheConsole.Log("No game Manager detected");
		}
		else if(gm_GO != null)
		{
			GameManager = gm_GO.GetComponent<TheScript>();
		}
		
		self_transform = self.GetComponent<TheTransform>();
		self_pos = self_transform.GlobalPosition;
		self_front = self_transform.ForwardDirection;
		self_up = self_transform.UpDirection;
		enemies = (List<TheGameObject>)GameManager.CallFunctionArgs("GetSlaveEnemies");

        if (targeting_go != null)
            targeting_script = targeting_go.GetScript("Targeting");
	}
	
	void Update () {
		//Step1 GET ENEMIES in front
		int j = 0;
		int aux_HUD_counter= 0;
		enemies_in_front.Clear();
		for(int i = 0; i < enemies.Count;i++)
		{
			isVisible = false;
			
			TheVector3 directionToTarget = enemies[i].GetComponent<TheTransform>().GlobalPosition - self_transform.GlobalPosition;
       		float angle = TheVector3.AngleBetween(self_transform.ForwardDirection, directionToTarget);
        	
 
        	if (TheMath.Abs (angle) < 90) {
            	isVisible = true;
       		}else {
           		isVisible = false;
        	}

			if(isVisible){
				enemies_in_front.Add(enemies[i]);

				j++; 
			}

		}
		
		//Step2
			//TO 2D	
		for(int l = 0; l <enemies_in_front.Count; l++)
		{
			if(prev_enemies_in_front.Count != enemies_in_front.Count)
			{
                if (targeting_script != null)
                {
                    TheGameObject target = (TheGameObject)targeting_script.CallFunctionArgs("GetTarget");
                    if (target != null)
                    {
                        if (target.GetComponent<TheTransform>() == enemies_in_front[l].GetComponent<TheTransform>())
                        {
                            markers.Add(TheResources.LoadPrefab("enemy_target"));
                        }
                    }
                    else
                    {
                        markers.Add(TheResources.LoadPrefab("enemy_marker"));
                    }

                }
                else
                {
                    markers.Add(TheResources.LoadPrefab("enemy_marker"));
                }

				if(canvas != null)
					markers[l].SetParent(canvas);
				else	
					TheConsole.Log("No canvas");
				//set pos
				markers[l].GetComponent<TheRectTransform>().Position = TheCamera.WorldPosToCameraPos(enemies_in_front[l].GetComponent<TheTransform>().GlobalPosition);
			}
			else{
				//Update the previous UI 
				markers[l].GetComponent<TheRectTransform>().Position = TheCamera.WorldPosToCameraPos(enemies_in_front[l].GetComponent<TheTransform>().GlobalPosition);
			}
			
		}

		prev_enemies_in_front = enemies_in_front;

	}
}