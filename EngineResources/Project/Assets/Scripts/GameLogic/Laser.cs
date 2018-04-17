using TheEngine;

public class Laser {

	void OnTriggerEnter(TheGameObject other_ship)
	{
		if(other_ship == null)
			return;
		
		if(other_ship.tag != "ShipEntity")
			return;

		TheScript ship_properties = other_ship.GetScript("ShipProperties");
		
		if(ship_properties != null)
			ship_properties.CallFunctionArgs("LaserHit");
	}
}