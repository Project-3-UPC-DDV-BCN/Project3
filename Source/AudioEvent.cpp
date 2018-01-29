#include "AudioEvent.h"
#include "Data.h"
#include "AudioSource.h"

AudioEvent::AudioEvent()
{
}

AudioEvent::~AudioEvent()
{
}

void AudioEvent::UnLoad()
{
}

void AudioEvent::Load(std::string * file, SoundBank * p, int id)
{
	// Add Data* data to function;


	/*
	bool succes = file->MoveToInsideArray("IncludedEvents", id);
	if (succes) {
		CONSOLE_DEBUG("Can be readed");
	}
	this->id = file->GetNumber("Id");
	this->name = file->GetString("Name");
	this->parent = p;*/
}

void AudioEvent::UIDraw(AudioSource* parent )
{
	//if (ImGui::CollapsingHeader(name.c_str())) {
	//	if (ImGui::Button("Play")) {
	//		//play event
	//		parent->obj->PlayEvent(name.c_str());
	//	}
	//	ImGui::SameLine();
	//	if (ImGui::Button("Stop")) {
	//		AK::SoundEngine::ExecuteActionOnEvent(name.c_str(), AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause);
	//	}
	//	ImGui::SameLine();
	//	if (ImGui::Button("Send")) {
	//		parent->SendEvent(name.c_str());
	//	}
	//}
}

