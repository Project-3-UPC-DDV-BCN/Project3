#include "ModuleParticleImporter.h"
#include "Data.h"
#include "ParticleData.h"
#include "ModuleFileSystem.h"
#include "ModuleResources.h"
#include "Application.h"
#include "ParticleData.h"

ModuleParticleImporter::ModuleParticleImporter(Application * app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
}

ModuleParticleImporter::~ModuleParticleImporter()
{
}

bool ModuleParticleImporter::Init(Data * editor_config)
{

	ParticleData* default_template = new ParticleData();
	default_template->LoadDefaultData(); 

	loaded_templates.push_back(default_template); 

	App->resources->AddParticleTemplate(default_template); 

	
	return true;
}

bool ModuleParticleImporter::Start()
{
	return true;
}

bool ModuleParticleImporter::CleanUp()
{
	return true;
}

void ModuleParticleImporter::SaveParticleData(ParticleData * new_template, const char* name)
{
	
}

std::string ModuleParticleImporter::ImportTemplate(std::string path)
{
	std::string file_name = App->file_system->GetFileName(path);
	std::string library_path = LIBRARY_PARTICLES_FOLDER + file_name;
	App->file_system->Copy(path, library_path);
	return library_path;
}

ParticleData * ModuleParticleImporter::LoadTemplateFromLibrary(std::string path)
{
	ParticleData* particle = new ParticleData();

	Data data;
	if (data.LoadBinary(path)) {
		particle->Load(data);
	}
	return particle;
}
