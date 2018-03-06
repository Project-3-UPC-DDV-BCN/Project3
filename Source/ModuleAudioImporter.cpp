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
	std::string name_without_path = App->file_system->GetFileName(path);

	if (App->file_system->FileExist(path))
	{
		App->file_system->Copy(path, LIBRARY_SOUNDBANK_FOLDER + name_without_path);
	}

	return (LIBRARY_SOUNDBANK_FOLDER + name_without_path);
}

SoundBankResource* ModuleAudioImporter::LoadSoundBankFromLibrary(std::string path)
{
	std::string name_without_extension = App->file_system->GetFileNameWithoutExtension(path);
	std::string name_without_path = App->file_system->GetFileName(path);

	if (App->file_system->FileExist(path))
	{
		SoundBankResource* sbk = new SoundBankResource();
		sbk->SetAssetsPath(ASSETS_SOUNDBANK_FOLDER + name_without_path);
		sbk->SetLibraryPath(LIBRARY_SOUNDBANK_FOLDER + name_without_path);
		sbk->SetName(name_without_path);
		sbk->LoadToMemory();

		return sbk;
	}

	return nullptr;
}
