#pragma once
#include "Component.h"
#include "ModuleAudio.h"
#include "SoundBank.h"
#include "Wwise.h"

class AudioEvent;

class ComponentAudioSource : 
	public Component 
{
public :
	ComponentAudioSource(GameObject* attached_gameobject);
	~ComponentAudioSource();

	bool Update();
	void PlayMusic(double audio_id);
	void PlayMusic(const char* name);
	void PlayEvent(uint id);
	bool PlayEvent(const char* event_name);
	bool StopEvent(const char * event_name);
	bool SendEvent(const char* name);
	AkGameObjectID GetID()const;
	void GetEvents();
	//void Serialize(JSON_File* doc);
	void ApplyReverb(float value, const char* bus);

	void Save(Data& data) const;
	void Load(Data& data);

	int* GetPickedEventPtr();

	std::vector<AudioEvent*> GetEventsVector() const;
	std::vector<AudioEvent*> GetEventsToPlayVector() const;

public:
	SoundBank* soundbank = nullptr;
private:
	std::vector<AudioEvent*>events;
	std::vector<AudioEvent*>events_to_play;
	int obj_to_load = -1;
	int picked_event = 0;

public:
	Wwise::SoundObject* obj = nullptr;
	bool is_combo = false;

};
