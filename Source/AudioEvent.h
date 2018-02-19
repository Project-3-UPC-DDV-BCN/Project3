#pragma once
#ifndef _AUDIOEVENT_H_
#define _AUDIOEVENT_H_

#include "ModuleAudio.h"

#include <string>
#include "SoundBank.h"

class SoundBank;
class ComponentAudioSource;
class JSON_File;

class AudioEvent {

public:

	AudioEvent();
	~AudioEvent();
	void UnLoad();

	void Load(JSON_File * file, SoundBank* p,int id);
	std::string name;
	uint id = 0;
	bool in_3D = false;
	void UIDraw(ComponentAudioSource* parent);

	AkPlayingID playing_id = 0L;
	AkCallbackFunc event_call_back = nullptr;
	SoundBank* parent = nullptr;
	

};










#endif