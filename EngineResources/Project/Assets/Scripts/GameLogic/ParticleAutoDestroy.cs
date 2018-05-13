using TheEngine;
using System.Collections.Generic;

public class ParticleAutoDestroy 
{
	public float destruction_time = 7.0f;

	TheTimer timer = new TheTimer();

	private bool to_start = true;
	private int iterations_waited = 0;
	
	TheGameObject self = null;
	
	void Start()
	{
		self = TheGameObject.Self;
	}
	
	void FireEmiters()
	{
		int childs_count = self.GetChildCount();

		for(int i = 0; i < childs_count; ++i)
		{
			TheGameObject child = self.GetChild(i);

			if(child != null)
			{
				TheParticleEmmiter emmiter = child.GetComponent<TheParticleEmmiter>();

				if(emmiter != null)
				{
					emmiter.Play();
				}
			}
		}
	}

	void Update () 
	{
		if(timer.ReadTime() > destruction_time && !to_start)
		{
			TheGameObject.Destroy(self);
			return;
		}

		if(to_start && iterations_waited == 2)
		{
			int childs_count = self.GetChildCount();

			FireEmiters();

			timer.Start();

			to_start = false;
		}	

		if(to_start)
			iterations_waited++;
	}
}