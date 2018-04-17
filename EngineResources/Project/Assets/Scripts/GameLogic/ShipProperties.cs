using TheEngine;

public class ShipProperties 
{
	public string ship_faction;
	private int life = 100;

	private TheGameObject game_manager = null;	
	private TheScript game_manager_script = null;

	void Init()
	{
		TheGameObject.Self.tag = "ShipEntity";
			
		game_manager = TheGameObject.Find("GameManager");
		
		if(game_manager != null)
			game_manager_script = game_manager.GetScript("GameManager");

		SetShipFaction(ship_faction);			
	}

	bool IsDead()
	{
		return life <= 0;
	}

	int GetLife()
	{
		return life;
	}

	string GetFaction()
	{
		return ship_faction;
	}
	
	void LaserHit()
	{
		DealDamage(10);
	}
	
	void DealDamage(int dmg)
	{
		if(dmg < 0)
			dmg = 0;

		life -= dmg;

		if(life < 0)
			life = 0;
	}
	
	void SetShipFaction(string faction)
	{	
		if(game_manager_script != null)
		{
			object[] args = {TheGameObject.Self};
			game_manager_script.CallFunctionArgs("RemoveShip", args);
			
			if(faction == "alliance")
			{
				game_manager_script.CallFunctionArgs("AddAllianceShip", args);
			}
			else if (faction == "empire")
			{
				game_manager_script.CallFunctionArgs("AddEmpireShip", args);
			}

			
		}
	}
}