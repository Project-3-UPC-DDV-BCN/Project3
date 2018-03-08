#include "ModuleMaterialImporter.h"
#include "Material.h"
#include "Texture.h"
#include "ModuleFileSystem.h"
#include "Application.h"
#include "Data.h"


ModuleMaterialImporter::ModuleMaterialImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Material_Importer";
}

ModuleMaterialImporter::~ModuleMaterialImporter()
{
}

std::string ModuleMaterialImporter::ImportMaterial(std::string path)
{
	std::string file_name = App->file_system->GetFileName(path);
	std::string library_path = LIBRARY_MATERIALS_FOLDER + file_name;
	App->file_system->Copy(path, library_path);
	return library_path;
}

Material * ModuleMaterialImporter::LoadMaterialFromLibrary(std::string path)
{
	Material* material = new Material();

	Data data;
	if (data.LoadBinary(path)) {
		material->Load(data);
		material->SetLibraryPath(path);
	}

	return material;
}


