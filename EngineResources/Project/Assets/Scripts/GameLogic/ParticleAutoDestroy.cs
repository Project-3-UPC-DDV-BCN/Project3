using TheEngine;
using System.Collections.Generic;

public class ParticleAutoDestroy 
{
	public float destruction_time = 7.0f;

	TheTimer timer = new TheTimer();

	public bool to_destroy = false;
	private int iterations_waited = 0;
	
	TheGameObject self = null;
	
	void Start()
	{
		self = TheGameObject.Self;
	}
	
	void Destroy()
	{
		to_destroy = true;
	}

	void Update () 
	{
		if(timer.ReadTime() > destruction_time)
		{
			TheGameObject.Destroy(self);
			return;
		}

		if(to_destroy && iterations_waited == 2)
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

			timer.Start();
		}	

		if(to_destroy)
			iterations_waited++;
	}
}