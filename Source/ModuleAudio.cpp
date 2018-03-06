#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleAudio.h"
#include "ModuleCamera3D.h"
#include "ModuleAudioImporter.h"
#include "JsonTool.h"

#include "GameObject.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "ComponentMeshRenderer.h"
#include "Mesh.h"

#include "SoundBank.h"
#include "ComponentDistorsionZone.h"
#include "ComponentAudioSource.h"
#include "ComponentListener.h"
#include "ModuleResources.h"

#include "ModuleScene.h"
#include "SoundBankResource.h"

#include "../EngineResources/Project/Assets/SoundBanks/Wwise_IDs.h"

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
	json = new JSONTool();

	return ret;
}

bool ModuleAudio::Start()
{
	//App->resources->CreateResource("./Project/Library/SoundBanks/Ship_Soundbank.bnk");

	SoundBank* sbk;
	GameObject* go = App->scene->CreateGameObject();
	go->SetName("Audio");
	ComponentAudioSource* as = (ComponentAudioSource*) go->AddComponent(Component::ComponentType::CompAudioSource);

	GameObject* go_ = App->scene->CreateGameObject();
	go_->AddComponent(Component::ComponentType::CompAudioListener);

	return true;
}

update_status ModuleAudio::PreUpdate(float dt)
{
	if ((App->IsStopped() || App->IsPaused()) && !stop_all)
	{
		AK::SoundEngine::StopAll();
		stop_all = true;
	}
	else if (App->IsPlaying() && stop_all)
	{
		stop_all = false;
	}

	if (!muted) {
		SetRTPvalue("Volume", volume);
		SetRTPvalue("Pitch", pitch);
	}
	else {
		SetRTPvalue("Volume", 0);
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleAudio::PostUpdate(float dt)
{
	AK::SoundEngine::RenderAudio();

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

void ModuleAudio::PushSoundBank(SoundBankResource * sbk)
{
	soundbanks.push_back(sbk);
}

std::vector<SoundBankResource*> ModuleAudio::GetSoundBanks() const
{
	return soundbanks;
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

		float3 cam_up = App->camera->GetCamera()->camera_frustum.Up();
		float3 cam_front = App->camera->GetCamera()->camera_frustum.Front();
		float3 cam_pos = App->camera->GetCamera()->camera_frustum.Pos();

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

Wwise::SoundObject* ModuleAudio::GetSoundObject(std::string obj_name)
{
	for (std::list<Wwise::SoundObject*>::iterator it = sound_obj.begin(); it != sound_obj.end(); ++it)
	{
		if ((*it)->GetName() == obj_name)
		{
			return *it;
		}
	}
	return nullptr;
}

Wwise::SoundObject* ModuleAudio::GetSoundObject(int obj_id)
{
	for (std::list<Wwise::SoundObject*>::iterator it = sound_obj.begin(); it != sound_obj.end(); ++it)
	{
		if ((*it)->GetID() == obj_id)
		{
			return *it;
		}
	}
	return nullptr;
}

void ModuleAudio::SetRTPvalue(const char * rtpc, float value)
{
	AK::SoundEngine::SetRTPCValue(rtpc, value);
}

void ModuleAudio::SetRTPvalue(const char * rtpc, float value, AkGameObjectID go_id)
{
	AK::SoundEngine::SetRTPCValue(rtpc, value, go_id);
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

void ModuleAudio::AddEnvironment(ComponentDistorsionZone * zone)
{
	environments.push_back(zone);
}

void ModuleAudio::DeleteEnvironment(ComponentDistorsionZone * zone)
{
	for (int i = 0; i < environments.size(); i++)
	{
		if (environments[i] == zone)
		{
			environments.erase((environments.begin() + i));
			break;
		}
	}
}

bool ModuleAudio::CheckEnvironments(GameObject * go)
{
	bool ret = false;

	ComponentTransform* t = (ComponentTransform*)go->GetComponent(Component::ComponentType::CompTransform);
	ComponentMeshRenderer* meshrend = (ComponentMeshRenderer*)go->GetComponent(Component::ComponentType::CompMeshRenderer);
	ComponentAudioSource* audio = (ComponentAudioSource*)go->GetComponent(Component::ComponentType::CompAudioSource);
	ComponentListener* list = (ComponentListener*)go->GetComponent(Component::ComponentType::CompAudioListener);
	if (!t || !audio || !meshrend)
		return ret;

	for (int i = 0; i < environments.size(); i++)
	{
		float value = 0.0;
		if (environments[i]->CheckCollision(meshrend->GetMesh()->box));
		{
			value = environments[i]->distorsion_value;
		}
		audio->ApplyReverb(value, environments[i]->bus.c_str());
	}

	if (list != nullptr) {
		for (int i = 0; i < environments.size(); i++)
		{
			float value = 0.0;
			if (environments[i]->CheckCollision(meshrend->GetMesh()->box));
			{
				value = environments[i]->distorsion_value;
			}
			list->ApplyReverb(value, environments[i]->bus.c_str());
		}
	}

	return ret; return false;
}

Wwise::SoundObject * ModuleAudio::GetCameraListener() const
{
	return camera_listener;
}

void ModuleAudio::SetCameraListener(Wwise::SoundObject * camera_listener)
{
	this->camera_listener = camera_listener;
}

ComponentListener * ModuleAudio::GetDefaultListener() const
{
	return default_listener;
}

void ModuleAudio::SetDefaultListener(ComponentListener* default_listener)
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

void ModuleAudio::SetListenerCreated(bool set)
{
	listener_created = set;
}

bool ModuleAudio::IsMuted()
{
	return muted;
}

void ModuleAudio::SetMute(const bool set)
{
	muted = set;
}

int ModuleAudio::GetVolume()
{
	return volume;
}

void ModuleAudio::SetVolume(const int volume)
{
	if (volume > 100)
		this->volume = 100;
	else if (volume < 0)
		this->volume = 0;
	else
		this->volume = volume;
}

int ModuleAudio::GetPitch()
{
	return pitch;
}

void ModuleAudio::SetPitch(const int pitch)
{
	if (pitch > 100)
		this->pitch = 100;
	else if (pitch < 0)
		this->pitch = 0;
	else
		this->pitch = pitch;
}

bool * ModuleAudio::IsMutedPtr()
{
	return &muted;
}

int * ModuleAudio::GetVolumePtr()
{
	return &volume;
}

int * ModuleAudio::GetPitchPtr()
{
	return &pitch;
}