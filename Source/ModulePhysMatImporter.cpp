#include "ModulePhysMatImporter.h"
#include "PhysicsMaterial.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Data.h"

ModulePhysMatImporter::ModulePhysMatImporter(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
}

ModulePhysMatImporter::~ModulePhysMatImporter()
{
}

std::string ModulePhysMatImporter::ImportPhysicsMaterial(std::string path)
{
	std::string file_name = App->file_system->GetFileName(path);
	std::string library_path = LIBRARY_PHYS_MATS_FOLDER + file_name;
	App->file_system->Copy(path, library_path);
	return library_path;
}

PhysicsMaterial * ModulePhysMatImporter::LoadPhysicsMaterialFromLibrary(std::string path)
{
	PhysicsMaterial* phys_mat = new PhysicsMaterial();

	Data data;
	if (data.LoadBinary(path)) {
		phys_mat->Load(data);
	}
	return phys_mat;
}

