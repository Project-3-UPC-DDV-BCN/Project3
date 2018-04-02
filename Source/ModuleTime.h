#pragma once
#include "Module.h"
#include "Timer.h"

class ModuleTime :
	public Module
{
public:
	ModuleTime(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleTime();
	bool Init();
	update_status PreUpdate(float dt);

	float GetGameDt() const;
	Uint32 GetTime();
	float GetPlayTime() const;

public:
	float time_scale;

private:
	float game_dt;
	float play_time;
	Timer time;
};

