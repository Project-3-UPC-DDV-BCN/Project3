#pragma once
#include "Window.h"

class Texture;

class GameBuildWindow :
	public Window
{
public:

	GameBuildWindow();
	~GameBuildWindow();

	void DrawWindow();

private:
	std::string starting_scene;
	Texture* game_icon;
	std::string game_name;
	std::string saving_path;
};

