using TheEngine;

public class Laser 
{

	private TheGameObject sender = null;

	void SetSender(TheGameObject send)
	{
		sender = send;
	}

	TheGameObject GetSender()
	{
		return sender;
	}

	void OnTriggerEnter(TheCollisionData coll)
	{
		if(coll == null)
			return;

		TheGameObject other_ship = coll.Collider.GetGameObject();

		if(other_ship == null)
			return;
		
		if(other_ship.tag != "ShipEntity")
			return;

		TheScript ship_properties = other_ship.GetScript("ShipProperties");
		
		if(ship_properties != null && sender != null)
		{
			object[] args = {sender};
			ship_properties.CallFunctionArgs("HitByShip", args);
		}
	}
}