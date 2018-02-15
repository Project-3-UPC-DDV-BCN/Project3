#include "AudioSource.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "AudioEvent.h"
#include "GameObject.h"
#include "ComponentTransform.h"

#include "Listener.h"

AudioSource::AudioSource(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("Audio Source");
	SetType(ComponentType::CompAudioSource);
	SetGameObject(attached_gameobject);

	ComponentTransform* trans = (ComponentTransform*) attached_gameobject->GetComponent(Component::CompTransform);

	obj = App->audio->CreateSoundObject(attached_gameobject->GetName().c_str(), trans->GetGlobalPosition());
	if (App->audio->GetSoundBank() != nullptr) {
		this->soundbank = App->audio->GetSoundBank();
		GetEvents();
	}

}

AudioSource::~AudioSource()
{
	//App->audio->UnRegisterGO(id);
}

bool AudioSource::Update()
{
	bool ret = true;

	if (App->IsPlaying()) 
	{
		ComponentTransform* trans = (ComponentTransform*)GetGameObject()->GetComponent(Component::CompTransform);

		if (trans)
		{
			float3 pos = trans->GetGlobalPosition();
			Quat rot = Quat::FromEulerXYZ(trans->GetGlobalRotation().x * DEGTORAD, trans->GetGlobalRotation().y * DEGTORAD, trans->GetGlobalRotation().z * DEGTORAD);

			float3 up = rot.Transform(float3(0, 1, 0));
			float3 front = rot.Transform(float3(0, 0, 1));

			up.Normalize();
			front.Normalize();

			obj->SetPosition(pos.x, pos.y, pos.z, front.x, front.y, front.z, up.x, up.y, up.z);
		}

		if (!events_to_play.empty()) {
			for (int i = 0; i < events_to_play.size(); i++) {
				PlayEvent(events_to_play[i]->name.c_str());
			}

			events_to_play.clear();
		}
	}

	return ret;
}

void AudioSource::PlayMusic(double audio_id)
{
	AK::SoundEngine::PostEvent(audio_id, obj->GetID(), AK_EnableGetMusicPlayPosition);
}

void AudioSource::PlayMusic(const char * name)
{
	AK::SoundEngine::PostEvent(name, obj->GetID(), AK_EnableGetMusicPlayPosition);
}

void AudioSource::PlayEvent(uint id)
{
	AK::SoundEngine::PostEvent(id, obj->GetID());
}

void AudioSource::PlayEvent(const char * event_name)
{
	if (obj != nullptr) 
	{
		obj->PlayEvent(event_name);
		CONSOLE_DEBUG("Event played: %s", event_name);
	}
	else
	{
		CONSOLE_DEBUG("Nullptr");
	}
}

void AudioSource::StopEvent(uint id)
{
}

void AudioSource::SendEvent(const char * name)
{
	for (int i = 0; i < events.size(); i++) {
		if (!strcmp(events[i]->name.c_str(),name)) {
			events_to_play.push_back(events[i]);
		}
	}
}

AkGameObjectID AudioSource::GetID() const
{
	return obj->GetID();
}

void AudioSource::UI_draw()
{
	if(events.empty())
	GetEvents();

	if(ImGui::CollapsingHeader("Audio Source")) {
		if (soundbank != nullptr) {
			ImGui::Text("SoundBank: %s", soundbank->name.c_str());
			if (ImGui::CollapsingHeader("Events")) {
				for (int i = 0; i < events.size(); i++) {
					events[i]->UIDraw(this);
				}
			}
		}
	}
}

void AudioSource::GetEvents()
{
	if (soundbank != nullptr) {
		for (int i = 0; i < soundbank->events.size(); i++) {

			events.push_back(soundbank->events[i]);

		}
	}
}

void AudioSource::ApplyReverb(float value, const char * bus)
{
	obj->SetAuxiliarySends(value, bus, App->audio->GetDefaultListener()->GetId());
}

void AudioSource::Save(Data & data) const
{
}

void AudioSource::Load(Data & data)
{
}

//void AudioSource::Serialize(JSON_File * doc)
//{
//	if (doc == nullptr)
//		return;
//
//	doc->SetNumber("type", type);
//	doc->SetNumber("ownerUID", (owner != nullptr) ? owner->GetUID() : -1);
//	doc->SetString("name", name);
//}


