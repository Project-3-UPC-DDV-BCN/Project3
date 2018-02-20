#pragma once

#pragma once
#include "Module.h"
#include <list>

class ParticleData; 

class ModuleParticleImporter :
	public Module
{
public:
	ModuleParticleImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleParticleImporter();

	bool Init(Data* editor_config = nullptr);
	bool Start();
	bool CleanUp();

	void SaveParticleData(ParticleData* new_template, const char* name);

	std::string ImportTemplate(std::string path);
	ParticleData* LoadTemplateFromLibrary(std::string path);

private:


	std::list<ParticleData*> loaded_templates; 


};

void Callback(const char* message, char* c);

