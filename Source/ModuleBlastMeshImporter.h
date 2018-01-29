#pragma once

#include "Module.h"
#include <string>

class BlastMesh;

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
	std::string ImportMesh(std::string path);
	BlastMesh* LoadMeshFromLibrary(std::string path);

private:
	Nv::Blast::ExtSerialization* serialization;
};

