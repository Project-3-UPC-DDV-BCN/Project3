using TheEngine;

public class SoftBoundaries {


	TheTransform trans;
	public TheText show_number_text; 
	public TheGameObject canvas;  

	public float limit_box_width; 
	public float limit_box_height;
	public float limit_box_depth;

	public int countdown_limit;

	private bool is_counting; 
	private float countdown_timer; 
	private int countdown_show_numb; 

	void Start ()
	{
		trans = TheGameObject.Self.GetComponent<TheTransform>(); 
		is_counting = false; 
		show_number_text.Text = "sdgf"; 
		countdown_timer = countdown_limit * 1000; 
		countdown_show_numb = countdown_limit; 
	}
	
	void Update () 
	{
		if(IsInside())
		{
			canvas.SetActive(false);
			if(is_counting == true)
			{
				countdown_timer = countdown_limit * 1000;
				countdown_show_numb = countdown_limit;
				 
			}
				 
			is_counting = false; 
		}
		else
			is_counting = true; 

		if(is_counting)
		{
			canvas.SetActive(true);
			countdown_timer--; 
			
			if(countdown_timer % 1000 == 0)
			{
				countdown_show_numb--; 
				show_number_text.Text = countdown_show_numb.ToString(); 
			}
		}
		
	}

	bool IsInside()
	{

		TheVector3 object_pos = trans.GlobalPosition; 

		if(object_pos.x < (limit_box_width/2) &&object_pos.x > -(limit_box_width/2))
			if(object_pos.y < (limit_box_height/2) &&object_pos.y > -(limit_box_height/2))
				if(object_pos.z < (limit_box_depth/2) &&object_pos.z > -(limit_box_depth/2))
					return true; 

		return false; 
	}
}