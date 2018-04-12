#pragma once
#include "Module.h"

class Texture;

class ModuleTextureImporter :
	public Module
{
public:
	ModuleTextureImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleTextureImporter();

	bool CleanUp();

	//Retuns the library path if created or an empty string
	std::string ImportTexture(std::string path);
	Texture* LoadTextureFromLibrary(std::string path);

	void CreateCubeMapTexture(std::string textures_path[6], uint& cube_map_id);
};

