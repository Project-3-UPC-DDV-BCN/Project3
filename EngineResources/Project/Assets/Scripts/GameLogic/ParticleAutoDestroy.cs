using TheEngine;
using System.Collections.Generic;

public class ParticleAutoDestroy 
{
	public float destruction_time = 4.0f;

	TheTimer timer = new TheTimer();

	void Start () 
	{	
		int childs_count = TheGameObject.Self.GetChildCount();

		for(int i = 0; i < childs_count; ++i)
		{
			TheGameObject child = TheGameObject.Self.GetChild(i);

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
	
	void Update () 
	{
		if(timer.ReadTime() > destruction_time)
		{
			TheGameObject.Destroy(TheGameObject.Self);
			return;
		}	
	}
}