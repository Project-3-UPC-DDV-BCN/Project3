#pragma once

#include "Module.h"
#include <string>

class BlastModel;

namespace Nv
{
	namespace Blast
	{
		class ExtSerialization;
	}
}

class ModuleBlastMeshImporter :
	public Module
{
public:
	ModuleBlastMeshImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleBlastMeshImporter();

	bool Init(Data* editor_config = nullptr);
	bool CleanUp();

	std::string ImportModel(std::string path);
	BlastModel* LoadModelFromLibrary(std::string path, bool loaded_from_scene);

private:
	Nv::Blast::ExtSerialization* serialization;
};

