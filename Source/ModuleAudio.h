#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "Include_Wwise.h"
#include "Geomath.h"
#include "Wwise.h"
//#include "DistorsionZone.h"
#include "SDL_mixer\include\SDL_mixer.h"
#include <list>

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define DEFAULT_VOLUME 50

class Listener;
class SoundBank;
class DistorsionZone;

class ModuleAudio : public Module
{
public:

	ModuleAudio(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleAudio();

	bool Init(Data* editor_config = nullptr);
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	// Game Objects
	Wwise::SoundObject* CreateSoundObject(const char* name, math::float3 position);

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);


	Wwise::SoundObject* GetCameraListener() const;
	void SetCameraListener(Wwise::SoundObject* camera_listener);
	Listener* GetDefaultListener() const;
	void SetDefaultListener(Listener* default_listener);

private:

	Wwise::SoundObject* camera_listener = nullptr;
	std::list <Wwise::SoundObject*> sound_obj;
	std::list<SoundBank*> soundbanks;
	std::vector<DistorsionZone*> environments;

	Listener* default_listener = nullptr;
	unsigned long listener_id = 1;
	unsigned long last_go_id = 100;

	Wwise::SoundObject*  emmiter;
	SoundBank* soundbank = nullptr;
	bool listener_created = false;
	int volume = DEFAULT_VOLUME;
	bool muted = false;
};

#endif // __ModuleAudio_H__