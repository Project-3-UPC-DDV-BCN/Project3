#pragma once
#include "Module.h"
#include "Globals.h"

#include <map>
#include <vector>

#define MAX_MOUSE_BUTTONS 5
#define MAX_GAMECONTROLLERS 4
#define NUM_CONTROLLER_BUTTONS 15

struct _SDL_GameController;
struct _SDL_Haptic;

enum JOYSTICK_MOVES
{
	LEFTJOY_LEFT = 0,
	LEFTJOY_RIGHT,
	LEFTJOY_UP,
	LEFTJOY_DOWN,
	RIGHTJOY_LEFT,
	RIGHTJOY_RIGHT,
	RIGHTJOY_UP,
	RIGHTJOY_DOWN,
	LEFT_TRIGGER,
	RIGHT_TRIGGER,
	JOY_MOVES_NULL
};

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

struct GamePad
{
	KEY_STATE				gamecontroller_buttons[NUM_CONTROLLER_BUTTONS];
	uint					joystick_moves[JOY_MOVES_NULL];
	int						pad_num = -1;
	int						id = -1;
	_SDL_GameController*	pad = nullptr;
	_SDL_Haptic*			haptic_system = nullptr;
};

class ModuleInput : public Module
{
public:
	
	ModuleInput(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleInput();

	bool Init(Data* editor_config = nullptr);
	update_status PreUpdate(float dt);
	bool CleanUp();

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	// Return GameController button state
	KEY_STATE GetControllerButton(int pad, int id) const;

	// Return the motion value (form 0 to 32767) for a joystick direction
	uint GetControllerJoystickMove(int pad, int id) const;

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

	void StoreStringKeys();

	SDL_Keycode StringToKey(std::string key);
	std::string KeyToString(SDL_Keycode key);

	JOYSTICK_MOVES StringToJoyMove(std::string axis);
	std::string JoyMoveToString(JOYSTICK_MOVES move);

	bool IsMouseDragging(int mouse_button);

private:

	void AxisReaction(int pad, int axis, int value);
	void AddController(int id);
	void RemoveController(int id);

	void ConnectGamePad(int instanceID);
	void DisconectGamePad(int instanceID);

private:
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];

	std::map<std::string, SDL_Keycode> stringKeys;
	std::map<std::string, JOYSTICK_MOVES> stringControllerAxis;

	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;
	//int mouse_z_motion;

	std::vector<GamePad*>	gamepads;
	int			connected_gamepads = 0;
	int			gamepad_connected[MAX_GAMECONTROLLERS];
};