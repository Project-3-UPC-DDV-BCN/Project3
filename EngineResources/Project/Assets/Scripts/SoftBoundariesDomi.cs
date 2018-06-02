using TheEngine;
using TheEngine.TheConsole;

public class SoftBoundariesDomi {

    TheTransform trans;

    public TheGameObject cdText;	
	private TheText texttext;

	TheScript entity_script = null;

    public float limitX;
    public float limitY;
    public float limitZ;

    
    public int timeLeft = 5;
    
	private float countdown;

	private float deltaTime; //optimizing delta time 

    private bool is_counting = false;

	private int suicide = 200; //damage to self

	private float offset = 0.1f;


	// Use this for initialization
	void Start () {
        trans = TheGameObject.Self.GetComponent<TheTransform>();
		
		entity_script = TheGameObject.Self.GetScript("EntityProperties");
		texttext = cdText.GetChild(0).GetComponent<TheText>();	

		cdText.SetActive(false);
		
		countdown = timeLeft;
        
	}
	
	// Update is called once per frame
	void Update () {

		deltaTime = TheTime.DeltaTime;

        if(!IsInside())
        {
            if(is_counting)//reset countdown
            {
        
				countdown -= deltaTime;

				texttext.Text = "Return to combat zone: " + (int)countdown;
            }
            else
            {
                is_counting = true;

				countdown = timeLeft;
				cdText.SetActive(true); //activate canvas
            }
             
            if(countdown <= 0.0f)
            {
				cdText.SetActive(false);
				object[] args = {suicide};
                entity_script.CallFunctionArgs("DealDamage", args);
				is_counting = false;

            }
        }
	}

    bool IsInside()
    {

        TheVector3 object_pos = trans.GlobalPosition;

        if (object_pos.x < (limitX / 2) && object_pos.x > -(limitX / 2))
        {
			if (object_pos.y < (limitY / 2) && object_pos.y > -(limitY / 2))
            { 
				if (object_pos.z < (limitZ / 2) && object_pos.z > -(limitZ / 2))
				{
					is_counting = false;	
					return true;
				}
			}   
		}    
                    

        return false;
    }

}