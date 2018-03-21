#include "ModulePrefabImporter.h"
#include "Prefab.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Data.h"
#include "ModuleScene.h"

ModulePrefabImporter::ModulePrefabImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
}

ModulePrefabImporter::~ModulePrefabImporter()
{
}

std::string ModulePrefabImporter::ImportPrefab(std::string path)
{
	std::string file_name = App->file_system->GetFileName(path);
	std::string library_path = LIBRARY_PREFABS_FOLDER + file_name;
	App->file_system->Copy(path, library_path);
	return library_path;
}

Prefab * ModulePrefabImporter::LoadPrefabFromLibrary(std::string path)
{
	Prefab* prefab = new Prefab();

	Data data;
	if (data.CanLoadAsJSON(path, ".jprefab"))
	{
		if (data.LoadJSON(path))
		{
			prefab->SetUID(data.GetUInt("UUID"));
			prefab->SetAssetsPath(data.GetString("assets_path"));
			prefab->SetLibraryPath(data.GetString("library_path"));
			prefab->SetName(data.GetString("prefab_name"));
		}
	}
	return prefab;
}
