using TheEngine; 

enum ship_side
{
	SHIP_SIDE_SITH,
	SHIP_SIDE_JEDI,
}; 

public class EntityProperties
{

	public TheGameObject canvas;
	public TheText countdown_number; 
	TheTransform trans;  

	//Properties
	private int score; 
	ship_side team; 

	//Soft Boundaries
	bool countdown_limits; 
	private float countdown_timer;
	public float countdown_limit; 
 
	public float boundary_limit;  

	void Start () 
	{
		 trans = TheGameObject.Self.GetComponent<TheTransform>(); 
		 countdown_timer = countdown_limit * 1000;  
		 score = 0; 
		 canvas.SetActive(false); 
	}
	
	void Update () 
	{
		if(IsInGameArea() == true)
		{
			countdown_limits = false; 
			canvas.SetActive(false); 
		}
		else 
		{
			countdown_limits = true; 
			canvas.SetActive(true); 

			if(IsInGameArea() == false)
			{
				countdown_limits = false;
			}
		}

		if(countdown_limits == true)
		{
			countdown_timer -= TheTime.DeltaTime;
			countdown_number.text = countdown_timer; 
		}

		if(true)//Projectile hited an enemie (?) Maybe in other script ? 
		{
			score += 20; 
		}
		else if(true)//Projectile hited an aliance (?)
		{
			//Do whatever the designer says, if someday he says something
		}
	}
	

	bool IsInGameArea()
	{
		if(trans.GlobalPosition.Length < boundary_limit)
			return true; 
		else 
			return false; 
	}
}