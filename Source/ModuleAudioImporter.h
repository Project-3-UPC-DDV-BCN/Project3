#pragma once

#include "Module.h"
#include <string>
#include "Resource.h"

class ModuleAudioImporter :
	public Module
{
public:
	ModuleAudioImporter(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleAudioImporter();

	bool Init(Data* editor_config = nullptr);
	bool CleanUp();

	std::string ImportSoundBank(std::string path);
	SoundBankResource* LoadSoundBankFromLibrary(std::string path);
private:

};