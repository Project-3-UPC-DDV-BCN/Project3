#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleAudio.h"
#include "ModuleCamera3D.h"
#include "ComponentCamera.h"
#include "SoundBank.h"


#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

ModuleAudio::ModuleAudio(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Audio";
} 

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init(Data* editor_config)
{
	CONSOLE_DEBUG("Loading Audio Mixer");
	
	bool ret = Wwise::InitWwise("English(US)");

	return ret;
}

bool ModuleAudio::Start()
{
	//Data bank_file;
	//std::string path_ = ASSETS_FOLDER;
	//path_ += "blend.json";
	//bank_file.LoadJSON(path_);

	//bank_file;

	return true;
}

update_status ModuleAudio::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleAudio::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	CONSOLE_DEBUG("Freeing sound FX, closing Mixer and Audio subsystem");

	delete camera_listener;
	if (soundbank != nullptr) {
		delete soundbank;
	}

	for (std::list<Wwise::SoundObject*>::iterator it = sound_obj.begin(); it != sound_obj.end(); ++it) {

		delete (*it);
	}
	return Wwise::CloseWwise();

	return true;
}

SoundBank * ModuleAudio::LoadSoundBank(std::string path)
{
	SoundBank* new_bank = new SoundBank();
	std::string bank_path = ASSETS_SOUNDBANK_FOLDER + path;
	Wwise::LoadBank(bank_path.c_str());

	std::string json_file = bank_path.substr(0, bank_path.find_last_of('.')) + ".json"; // Changing .bnk with .json
	GetBankInfo(json_file, new_bank);
	soundbanks.push_back(new_bank);
	soundbank = new_bank;
	return new_bank;
}

unsigned int ModuleAudio::GetBankInfo(std::string path, SoundBank *& bank)
{

	unsigned int ret = 0;
	
	Data bank_file;

	if (!bank_file.LoadJSON(path.c_str()))
	{
		CONSOLE_DEBUG("Error reading bank json file");
	}
	else
	{

	}

	
	/*
	bank_file->RootObject();

	if (bank_file == nullptr) {
		LOG_OUT("Error reading bank json file");
	}
	else {
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

	}*/
	return ret;
}

Wwise::SoundObject * ModuleAudio::CreateSoundObject(const char * name, math::float3 position)
{
	Wwise::SoundObject* ret = Wwise::CreateSoundObj(last_go_id++, name, position.x, position.y, position.z);
	sound_obj.push_back(ret);

	return ret;
}

Wwise::SoundObject * ModuleAudio::CreateListener(const char * name, math::float3 position)
{
	Wwise::SoundObject* ret;

	if (!listener_created) {

		float3 cam_up = App->camera->GetCamera()->camera_frustum.up;
		float3 cam_front = App->camera->GetCamera()->camera_frustum.front;
		float3 cam_pos = App->camera->GetCamera()->camera_frustum.pos;

		ret = Wwise::CreateSoundObj(0, "Listener", cam_pos.x, cam_pos.y, cam_pos.z, true);
		ret->SetPosition(cam_pos.x, cam_pos.y, cam_pos.z, cam_front.x, cam_front.y, cam_front.z, cam_up.x, cam_up.y, cam_up.z);

		sound_obj.push_back(ret);
		listener_created = true;

	}
	else {
		CONSOLE_DEBUG("It exist a listener already!");
		ret = nullptr;
	}

	return ret;
}

void ModuleAudio::SetRTPvalue(const char * rtpc, float value)
{
	AK::SoundEngine::SetRTPCValue(rtpc, value);
}

void ModuleAudio::StopAllEvents()
{
	for (int i = 0; i < soundbank->events.size(); i++) {
		AK::SoundEngine::ExecuteActionOnEvent(soundbank->events[i]->name.c_str(), AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause);
	}
}

void ModuleAudio::ImGuiDraw()
{
}

Wwise::SoundObject * ModuleAudio::GetCameraListener() const
{
	return camera_listener;
}

void ModuleAudio::SetCameraListener(Wwise::SoundObject * camera_listener)
{
	this->camera_listener = camera_listener;
}

Listener * ModuleAudio::GetDefaultListener() const
{
	return default_listener;
}

void ModuleAudio::SetDefaultListener(Listener* default_listener)
{
	this->default_listener = default_listener;
}

Wwise::SoundObject * ModuleAudio::GetEmmiter() const
{
	return emmiter;
}

void ModuleAudio::SetEmmiter(Wwise::SoundObject * emmiter)
{
	this->emmiter = emmiter;
}

SoundBank * ModuleAudio::GetSoundBank() const
{
	return soundbank;
}

void ModuleAudio::SetSoundBank(SoundBank * soundbank)
{
	this->soundbank = soundbank;
}
