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
	std::string play = "Play##"; play += name.c_str();
	std::string stop = "Stop##"; stop += name.c_str();
	std::string send = "Send##"; send += name.c_str();

	if (ImGui::Button(play.c_str(), ImVec2(70, 25))) {
		//play event
		parent->obj->PlayEvent(name.c_str());
	}
	ImGui::SameLine();
	if (ImGui::Button(stop.c_str(), ImVec2(70, 25))) {
		AK::SoundEngine::ExecuteActionOnEvent(name.c_str(), AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause);
	}
	ImGui::SameLine();
	if (ImGui::Button(send.c_str(), ImVec2(70, 25))) {
		parent->SendEvent(name.c_str());
	}
}

