using TheEngine;

public class Tank_Movement {

	public TheGameObject tank;
    public int movement_speed = 5;
    public int rotation_speed = 100; 
	
	void Start () {

	}
	
	void Update () {
		if(TheInput.IsKeyRepeat("UP_ARROW") || TheInput.IsKeyRepeat("w"))
        {
            Move(tank.GetComponent<TheTransform>().ForwardDirection);
        }
        else if (TheInput.IsKeyRepeat("DOWN_ARROW") || TheInput.IsKeyRepeat("s"))
        {
            Move(tank.GetComponent<TheTransform>().ForwardDirection * -1);
        }
        if (TheInput.IsKeyRepeat("LEFT_ARROW") || TheInput.IsKeyRepeat("a"))
        {
            Rotate(TheVector3.Up);
        }
        else if (TheInput.IsKeyRepeat("RIGHT_ARROW") || TheInput.IsKeyRepeat("d"))
        {
            Rotate(TheVector3.Down);
        }
	}

    void Move(TheVector3 movement)
    {
        tank.GetComponent<TheTransform>().GlobalPosition += movement * movement_speed * Time.DeltaTime;
    }

    void Rotate(TheVector3 rotation)
    {
        tank.GetComponent<TheTransform>().GlobalRotation += rotation * rotation_speed * Time.DeltaTime;
    }
}