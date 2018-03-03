#include "ModuleAudioImporter.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "SoundBankResource.h"
#include "Data.h"


ModuleAudioImporter::ModuleAudioImporter(Application * app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Audio_Importer";
}

ModuleAudioImporter::~ModuleAudioImporter()
{
}

bool ModuleAudioImporter::Init(Data * editor_config)
{
	return true;
}

bool ModuleAudioImporter::CleanUp()
{
	return true;
}

std::string ModuleAudioImporter::ImportSoundBank(std::string path)
{
	std::string name_without_extension = App->file_system->GetFileNameWithoutExtension(path);

	if (App->file_system->FileExist(ASSETS_SOUNDBANK_FOLDER + path))
	{
		SoundBankResource* sbk = new SoundBankResource();
		sbk->SetAssetsPath(ASSETS_SOUNDBANK_FOLDER + path);
		sbk->SetLibraryPath(LIBRARY_SOUNDBANK_FOLDER + path);
		sbk->SetName(path);
		sbk->LoadToMemory();

		Data data;
		sbk->Save(data);
		sbk->CreateMeta();
		App->file_system->Copy(ASSETS_SOUNDBANK_FOLDER + path, LIBRARY_SOUNDBANK_FOLDER + path);

		RELEASE(sbk);
	}

	return std::string();
}
