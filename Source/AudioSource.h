#pragma once
#include "Component.h"
#include "ModuleAudio.h"
#include "SoundBank.h"
#include "Wwise.h"

class AudioEvent;

class AudioSource : 
	public Component 
{
public :
	AudioSource(GameObject* attached_gameobject);
	~AudioSource();

	bool Update();
	void PlayMusic(double audio_id);
	void PlayMusic(const char* name);
	void PlayEvent(uint id);
	void PlayEvent(const char* event_name);
	void StopEvent(uint id);
	void SendEvent(const char* name);
	AkGameObjectID GetID()const;
	void UI_draw();
	void GetEvents();
	//void Serialize(JSON_File* doc);
	void ApplyReverb(float value, const char* bus);

public:
	SoundBank* soundbank = nullptr;
private:

	std::vector<AudioEvent*>events;

	std::vector<AudioEvent*>events_to_play;
public:
	Wwise::SoundObject* obj;
};
