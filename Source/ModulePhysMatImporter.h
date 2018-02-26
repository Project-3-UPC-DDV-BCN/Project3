#pragma once
#include "Module.h"

class PhysicsMaterial;

class ModulePhysMatImporter :
	public Module
{
public:
	ModulePhysMatImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModulePhysMatImporter();

	//Retuns the library path if created or an empty string
	std::string ImportPhysicsMaterial(std::string path);
	PhysicsMaterial* LoadPhysicsMaterialFromLibrary(std::string path);
};

