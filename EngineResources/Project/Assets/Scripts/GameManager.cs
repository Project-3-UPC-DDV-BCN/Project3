using TheEngine;

public class GameManager
{

	public TheGameObject tie_fighter; 
	public TheGameObject x_wing; 
	public TheGameObject y_wing; 
	public TheGameObject sentinel_class; 
	public TheGameObject landing_class; 

	private bool is_alliance; 

	void Start ()
	{
		if(is_alliance)
			CreateShips("Alliance"); 
		else 
			CreateShips("Empire"); 
	}
	
	void Update () {
		
	}

	void SetTeam(string team)
	{
		if(team == "Alliance")
			is_alliance = true; 
		else
			is_alliance = false; 
	}

	void CreateShips(string team)
	{
		if(team == "Alliance")
		{
			// 18 x_wing
			// 26 tie_fighter
			// 12 ywing
			// 14 landing

			for(int i = 0; i < 18; i++)
			{

			}

			for(int i = 0; i < 26; i++)
			{

			}
		}
		else
		{
			// 26 x_wing
			// 18 tie_fighter
			// 12 y wing
			// 14 landing

			for(int i = 0; i < 26; i++)
			{

			}

			for(int i = 0; i < 18; i++)
			{

			}
		}

		for(int i = 0; i < 12; i++)
		{

		}

		for(int i = 0; i < 14; i++)
		{

		}
	}
}