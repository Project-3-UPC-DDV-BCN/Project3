#pragma once

#include "Window.h"
#include "MathGeoLib\Math\float2.h"

class GameWindow :
	public Window
{
public:
	GameWindow();
	virtual ~GameWindow();

	void DrawWindow();

	float2 GetSize() const;

private:
	float game_scene_width;
	float game_scene_height;
};

