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

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

private:

	Mix_Music*			music;
	std::list<Mix_Chunk*>	fx;
};

#endif // __ModuleAudio_H__