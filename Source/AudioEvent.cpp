#include "AudioEvent.h"
#include "JsonTool.h"
#include "ComponentAudioSource.h"

AudioEvent::AudioEvent()
{
}

AudioEvent::~AudioEvent()
{
}

void AudioEvent::UnLoad()
{
}

void AudioEvent::Load(JSON_File * file, SoundBank * p, int id)
{
	bool succes = file->MoveToInsideArray("IncludedEvents", id);
	if (succes) {
		CONSOLE_DEBUG("Can be readed");
	}
	this->id = file->GetNumber("Id");
	this->name = file->GetString("Name");
	this->parent = p;
}

void AudioEvent::UIDraw(ComponentAudioSource* parent )
{
	if (ImGui::CollapsingHeader(name.c_str())) {
		if (ImGui::Button("Play")) {
			//play event
			parent->obj->PlayEvent(name.c_str());
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop")) {
			AK::SoundEngine::ExecuteActionOnEvent(name.c_str(), AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause);
		}
		ImGui::SameLine();
		if (ImGui::Button("Send")) {
			parent->SendEvent(name.c_str());
		}
	}
}

