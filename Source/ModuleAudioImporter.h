#pragma once

#include "Module.h"
#include <string>

class ModuleAudioImporter :
	public Module
{
public:
	ModuleAudioImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleAudioImporter();

	bool Init(Data* editor_config = nullptr);
	bool CleanUp();

	std::string ImportSoundBank(std::string path);
	
private:

};