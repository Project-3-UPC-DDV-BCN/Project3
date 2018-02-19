#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "Include_Wwise.h"
#include "Geomath.h"
#include "Wwise.h"

#include <string>
#include <list>

#define DEFAULT_MUSIC_FADE_TIME 2.0f
#define DEFAULT_VOLUME 50

class ComponentListener;
class GameObject;
class SoundBank;
class ComponentDistorsionZone;
class JSONTool;

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


	SoundBank* LoadSoundBank(std::string path);
	unsigned int GetBankInfo(std::string path, SoundBank* &bank);

	// Game Objects
	Wwise::SoundObject* CreateSoundObject(const char* name, math::float3 position);
	Wwise::SoundObject* CreateListener(const char* name, math::float3 position);

	void SetRTPvalue(const char* rtpc, float value);
	void StopAllEvents();
	void ImGuiDraw();

	//Environments
	void AddEnvironment(ComponentDistorsionZone* zone);
	void DeleteEnvironment(ComponentDistorsionZone* zone);
	bool CheckEnvironments(GameObject* go);

	Wwise::SoundObject* GetCameraListener() const;
	void SetCameraListener(Wwise::SoundObject* camera_listener);
	ComponentListener* GetDefaultListener() const;
	void SetDefaultListener(ComponentListener* default_listener);
	Wwise::SoundObject* GetEmmiter() const;
	void SetEmmiter(Wwise::SoundObject* emmiter);
	SoundBank* GetSoundBank() const;
	void SetSoundBank(SoundBank* soundbank);

private:

	Wwise::SoundObject* camera_listener = nullptr;
	std::list <Wwise::SoundObject*> sound_obj;
	std::list<SoundBank*> soundbanks;
	std::vector<ComponentDistorsionZone*> environments;

	ComponentListener* default_listener = nullptr;
	unsigned long listener_id = 1;
	unsigned long last_go_id = 100;

	Wwise::SoundObject*  emmiter;
	SoundBank* soundbank = nullptr;
	bool listener_created = false;
	int volume = DEFAULT_VOLUME;
	bool muted = false;

	JSONTool* json = nullptr;
};

#endif // __ModuleAudio_H__