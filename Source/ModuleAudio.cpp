#include "Globals.h"
#include "Application.h"
#include "ModuleAudio.h"

#pragma comment( lib, "SDL_mixer/libx86/SDL2_mixer.lib" )

ModuleAudio::ModuleAudio(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Audio";
} 

// Destructor
ModuleAudio::~ModuleAudio()
{}

// Called before render is available
bool ModuleAudio::Init(Data* editor_config)
{
	CONSOLE_DEBUG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		CONSOLE_DEBUG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	// load support for the OGG format
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		CONSOLE_DEBUG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		ret = false;
	}

	//Initialize SDL_mixer
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		CONSOLE_DEBUG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		ret = false;
	}

	return ret;
}

bool ModuleAudio::Start()
{
	return false;
}

update_status ModuleAudio::PreUpdate(float dt)
{
	return update_status();
}

update_status ModuleAudio::PostUpdate(float dt)
{
	return update_status();
}

// Called before quitting
bool ModuleAudio::CleanUp()
{
	CONSOLE_DEBUG("Freeing sound FX, closing Mixer and Audio subsystem");

	delete camera_listener;
	if (soundbank != nullptr) {
		delete soundbank;
	}

	for (std::list<Wwise::SoundObject*>::iterator it = sound_obj.begin(); it != sound_obj.end(); ++it) {

		delete (*it);
	}
	return Wwise::CloseWwise();

	return true;
}

Wwise::SoundObject * ModuleAudio::CreateSoundObject(const char * name, math::float3 position)
{
	Wwise::SoundObject* ret = Wwise::CreateSoundObj(last_go_id++, name, position.x, position.y, position.z);
	sound_obj.push_back(ret);

	return ret;
}

Wwise::SoundObject * ModuleAudio::GetCameraListener() const
{
	return camera_listener;
}

void ModuleAudio::SetCameraListener(Wwise::SoundObject * camera_listener)
{
	this->camera_listener = camera_listener;
}

Listener * ModuleAudio::GetDefaultListener() const
{
	return default_listener;
}

void ModuleAudio::SetDefaultListener(Listener* default_listener)
{
	this->default_listener = default_listener;
}
