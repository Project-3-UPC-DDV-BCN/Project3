#pragma once
#include "Globals.h"
#include "PerfTimer.h"
#include <string>
class Application;
struct PhysBody3D;
class Data;

class Module
{
private :
	bool enabled;
public:
	std::string name;
	PerfTimer ms_timer;
	Application* App;
	bool is_game;

	Module(Application* parent, bool start_enabled = true, bool is_game = false) : App(parent), enabled(start_enabled),is_game(is_game)
	{}

	virtual ~Module()
	{}

	virtual bool Init(Data* editor_config = nullptr) 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual void SaveData(Data* data)
	{}
};