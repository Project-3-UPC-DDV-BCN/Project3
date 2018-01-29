#include "AudioSource.h"
#include "Application.h"
#include "ModuleAudio.h"
#include "AudioEvent.h"

AudioSource::AudioSource(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("Audio Source");
	SetType(ComponentType::CompAudioSource);
	SetGameObject(attached_gameobject)

	obj = App->audio->CreateSoundObject(own->GetName(), trans->GetPosition());
	if (App->audio->soundbank != nullptr) {
		this->soundbank = App->audio->soundbank;
		GetEvents();
	}

}

AudioSource::~AudioSource()
{
	//App->audio->UnRegisterGO(id);
}

bool AudioSource::Update()
{

	if (App->tm->GetGameState() == IN_PLAY) {

		Transform* trans = (Transform*)GetOwner()->FindComponentbyType(TRANSFORM);


		if (trans)
		{


			float3 pos = trans->GetPosition();
			Quat rot = trans->GetRotation();

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
	if (obj == nullptr) {
		CONSOLE_DEBUG("Nullptr");
	}
	obj->PlayEvent(event_name);
	CONSOLE_DEBUG("Event played: %s", event_name);
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

//void AudioSource::Serialize(JSON_File * doc)
//{
//	if (doc == nullptr)
//		return;
//
//	doc->SetNumber("type", type);
//	doc->SetNumber("ownerUID", (owner != nullptr) ? owner->GetUID() : -1);
//	doc->SetString("name", name);
//}


