#include "ModuleTime.h"
#include "Application.h"


ModuleTime::ModuleTime(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Time";

	time_scale = 1.0f;
	game_dt = 0.0f;
}


ModuleTime::~ModuleTime()
{
}

bool ModuleTime::Init()
{
	time.Start();
	return true;
}

update_status ModuleTime::PreUpdate(float dt)
{
	game_dt = dt * time_scale;

	return UPDATE_CONTINUE;
}

float ModuleTime::GetGameDt() const
{
	return game_dt;
}

Uint32 ModuleTime::GetTime()
{
	return time.Read();
}
