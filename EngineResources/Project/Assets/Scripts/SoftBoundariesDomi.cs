using TheEngine;

public class SoftBoundariesDomi {

    TheTransform trans;
    public TheGameObject canv;

    public TheText cdText;

    public float limitX;
    public float limitY;
    public float limitZ;

    private float check = 1.0f; //time between checks

    public int timeLeft = 5;
    private int timeToShow;

    private float countdownTimeMs; //seconds to ms
    

    private bool is_counting;


	// Use this for initialization
	void Start () {
        trans = TheGameObject.Self.GetComponent<TheTransform>();
        

        is_counting = false;
        countdownTimeMs = timeLeft * 1000;
        timeToShow = timeLeft;
        
	}
	
	// Update is called once per frame
	void Update () {
        //check -= Time.deltaTime;
        if(check < 0)
        {
            if(IsInside())
            {
                
                if(is_counting)//reset countdown
                {
                    countdownTimeMs = timeLeft * 1000;
                    timeToShow = timeLeft;

                }
                is_counting = false;
            }
            else
            {
                is_counting = true;
            }
                

            if (is_counting)
            {
                //countdownTimeMs -= Time.deltaTime;
                //cdText.text = "Return to combat zone in " + countdownTimeMs;
                if(countdownTimeMs % 1000 == 0)
                {
                    timeToShow--;
                }

              
            }
            if(timeLeft <= 0)
            {
                //die
            }

            check = 1.0f;
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