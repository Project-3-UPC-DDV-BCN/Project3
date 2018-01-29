#pragma once
#ifndef _AUDIOEVENT_H_
#define _AUDIOEVENT_H_

#include "ModuleAudio.h"

#include <string>
#include "SoundBank.h"

class SoundBank;
class AudioSource;

class AudioEvent {

public:

	AudioEvent();
	~AudioEvent();
	void UnLoad();

	void Load(std::string* file, SoundBank* p,int id);
	std::string name;
	uint id = 0;
	bool in_3D = false;
	void UIDraw(AudioSource* parent);

	AkPlayingID playing_id = 0L;
	AkCallbackFunc event_call_back = nullptr;
	SoundBank* parent = nullptr;
	

};










#endif