#include "SoundBankResource.h"
#include <ctime>
#include "Data.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "SoundBank.h"
#include "JsonTool.h"

SoundBankResource::SoundBankResource()
{
	SetType(Resource::SoundBankResource);
	json = new JSONTool();
}

SoundBankResource::~SoundBankResource()
{
}

void SoundBankResource::Save(Data & data) const
{
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("soundbank_name", GetName());
	data.AddUInt("UUID", GetUID());
}

bool SoundBankResource::Load(Data& data)
{
	SetUID(data.GetUInt("UUID"));
	SetAssetsPath(data.GetString("assets_path"));
	SetLibraryPath(data.GetString("library_path"));
	SetName(data.GetString("soundbank_name"));

	return true;
}

void SoundBankResource::CreateMeta() const
{
	time_t now = time(0);
	char* dt = ctime(&now);

	Data data;
	data.AddInt("Type", GetType());
	data.AddUInt("UUID", GetUID());
	data.AddString("Time_Created", dt);
	data.AddString("Library_path", GetLibraryPath());
	data.SaveAsMeta(GetAssetsPath());
}

void SoundBankResource::LoadToMemory()
{
	SoundBank* new_bank = new SoundBank();
	std::string bank_path = ASSETS_SOUNDBANK_FOLDER + GetName();
	Wwise::LoadBank(bank_path.c_str());

	std::string json_file = bank_path.substr(0, bank_path.find_last_of('.')) + ".json"; // Changing .bnk with .json
	GetBankInfo(json_file, new_bank);
	App->audio->PushSoundBank(new_bank);
}

void SoundBankResource::UnloadFromMemory()
{
	std::vector<SoundBank*> soundbanks = App->audio->GetSoundBanks();

	for (int i = 0; i < soundbanks.size(); ++i)
	{
		if (soundbanks[i] == soundbank) 
		{
			soundbanks.erase(soundbanks.begin() + i);
		}
	}
	soundbank = nullptr;
}

unsigned int SoundBankResource::GetBankInfo(std::string path, SoundBank *& bank)
{
	unsigned int ret = 0;

	JSON_File * bank_file = json->LoadJSON(path.c_str());
	bank_file->RootObject();

	if (bank_file == nullptr)
	{
		CONSOLE_DEBUG("Error reading bank json file");
	}
	else
	{
		bank_file->ChangeObject("SoundBanksInfo");
		int n_banks = bank_file->ArraySize("SoundBanks");
		for (int i = 0; i < n_banks; i++) {
			bank_file->RootObject();
			bank_file->ChangeObject("SoundBanksInfo");
			bank_file->MoveToInsideArray("SoundBanks", i);
			ret = bank->id = bank_file->GetNumber("Id");
			bank->name = bank_file->GetString("ShortName");
			bank->path = bank_file->GetString("Path");

			//bank_file->RootObject();
			int n_events = bank_file->ArraySize("IncludedEvents");
			for (int j = 0; j < n_events; j++) {

				//create new event and load it
				AudioEvent* new_event = new AudioEvent();
				bank_file->RootObject();
				bank_file->ChangeObject("SoundBanksInfo");
				bank_file->MoveToInsideArray("SoundBanks", i);
				new_event->Load(bank_file, bank, j);
				bank->events.push_back(new_event);
			}
		}
	}

	return ret;
}

SoundBank * SoundBankResource::GetSoundBank() const
{
	return soundbank;
}

void SoundBankResource::SetSoundBank(SoundBank * sbk)
{
	soundbank = sbk;
}
