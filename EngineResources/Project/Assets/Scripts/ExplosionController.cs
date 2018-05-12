using TheEngine;
using System.Collections.Generic; 

public class ExplosionController 
{

	private TheTimer time;
	private TheTimer time_from_start;  

	List<TheParticleEmmiter> childs; 

	public int start_in; 
	private int frame_counter; 

	public float fire1_start; 
	public float fire2_start; 

	private bool started; 
	private bool fire1_started; 
	private bool fire2_started; 

	void Start () 
	{
		time.Start(); 
		childs = new List<TheParticleEmmiter>(); 
		FillChildList(); 
	}

	void FillChildList()
	{
		for(int i = 0; i < TheGameObject.Self.GetChildCount(); i++)
		{
			childs.Add(TheGameObject.Self.GetChild(i).GetComponent<TheParticleEmmiter>());
		}
	}
	
	void Update () 
	{

		for(int i = 0; i < TheGameObject.Self.GetChildCount(); i++)
		{
			childs[i].Play(); 
		}
		/*frame_counter++; 

		if(frame_counter > start_in && started == false)
		{
			started = true;
			time.Start(); 

			childs[0].Play(); 
			childs[1].Play();
		}
			 
		if(time.ReadTime() > fire1_start && fire1_started == false)
		{
			childs[2].Play();
			fire1_started = true; 
		}

		if(time.ReadTime() > fire2_start && fire2_started == false)
		{
			childs[3].Play();
			fire2_started = true; 
		}*/ 
	    		
	}


}