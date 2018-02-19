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

void AudioEvent::UIDraw(ComponentAudioSource* parent)
{
	if (ImGui::Button("Play", ImVec2(70, 25))) {
		//play event
		parent->obj->PlayEvent(name.c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop", ImVec2(70, 25))) {
		AK::SoundEngine::ExecuteActionOnEvent(name.c_str(), AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause);
	}
	ImGui::SameLine();
	if (ImGui::Button("Send", ImVec2(70, 25))) {
		parent->SendEvent(name.c_str());
	}
}

