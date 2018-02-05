#pragma once
#include "Module.h"

class Shader;

class ModuleShaderImporter :
	public Module
{
public:
	ModuleShaderImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleShaderImporter();

	//Retuns the library path if created or an empty string
	std::string ImportShader(std::string path);
	Shader* LoadShaderFromLibrary(std::string path);

private:
	bool CompileShader(std::string path, std::string& library);
};

