using TheEngine;

public class SoftBoundariesDomi {

    TheTransform trans;
    public TheGameObject canv;

    public TheText cdText;

	TheScript entity_script = TheGameObject.Self.GetScript("EntityProperties");

    public float limitX;
    public float limitY;
    public float limitZ;

    
    public int timeLeft = 5;
    private float countdownTimeMs; //seconds to ms
	private int timeToShow;
    

    private bool is_counting;

	private int suicide = 200;


	// Use this for initialization
	void Start () {
        trans = TheGameObject.Self.GetComponent<TheTransform>();
        
        is_counting = false;
        countdownTimeMs = timeLeft * 1000;
		timeToShow = timeLeft;
        
	}
	
	// Update is called once per frame
	void Update () {

        if(IsInside())
        {
                
            if(is_counting)//reset countdown
            {
                countdownTimeMs = timeLeft * 1000;
                is_counting = false;
				timeToShow = timeLeft;

				countdownTimeMs -= TheTime.DeltaTime;

				if((countdownTimeMs % 1000.0 < (timeToShow + 0.4)) && (countdownTimeMs % 1000.0 > (timeToShow - 0.4)))
                {
                    timeToShow--;
                }

				//cdText.text = "Return to combat zone: " + timeToShow;
            }
            else
            {
                is_counting = true;
            }
             
            if(timeLeft <= 0)
            {
               //entity_script.CallFunction(DealDamage(suicide));
            }

        }
		
	}
    bool IsInside()
    {

        TheVector3 object_pos = trans.GlobalPosition;

        if (object_pos.x < (limitX / 2) && object_pos.x > -(limitX / 2))
            if (object_pos.y < (limitY / 2) && object_pos.y > -(limitY / 2))
                if (object_pos.z < (limitZ / 2) && object_pos.z > -(limitZ / 2))
                    return true;

        return false;
    }

}