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
				if(entity_script != null)
				{
					object[] args = {0};
                	entity_script.CallFunctionArgs("SetLife", args);
					entity_script.SetBoolField("is_dead", true);
					
				}
				
				is_counting = false;

            }
        }
		else
		{
			if(cdText.IsActive())
			{
				cdText.SetActive(false);
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