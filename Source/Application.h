#pragma once

#include "Globals.h"
#include "MathGeoLib\Algorithm\Random\LCG.h"
#include <list>
#include "Timer.h"

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleScene;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleEditor;
class ModuleResources;
class ModuleTime;
class ModuleFileSystem;
class Data;
class TagsAndLayers;
class ModuleMeshImporter;
class ModuleTextureImporter;
class ModulePrefabImporter;
class ModuleParticleImporter; 
class ModuleMaterialImporter;
class ModuleScriptImporter;
class ModuleShaderImporter;
struct SDL_Surface;

class Application
{

public:

	enum EnGineCursors
	{
		ENGINE_CURSOR_ADD, ENGINE_CURSOR_ARROW, ENGINE_CURSOR_IBEAM, ENGINE_CURSOR_WAIT, ENGINE_CURSOR_WAITARROW, ENGINE_CURSOR_HAND, ENGINE_CURSOR_NO
	};

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	LCG& RandomNumber();
	void CapFPS(int max_fps);
	void SetCustomCursor(EnGineCursors cursor_type);
	void CreateEngineData(Data* data);
	void UpdateStep();

	void Play();
	void Pause();
	void UnPause();
	void Stop();
	bool IsPlaying();
	bool IsPaused();
	bool IsStopped();

	enum EngineState {
		OnPlay, OnPause, OnStop
	};

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleScene* scene;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleResources* resources;
	ModuleTime* time;
	TagsAndLayers* tags_and_layers;
	ModuleFileSystem* file_system;
	ModuleMeshImporter* mesh_importer;
	ModuleTextureImporter* texture_importer;
	ModulePrefabImporter* prefab_importer;
	ModuleMaterialImporter* material_importer;
	ModuleScriptImporter* script_importer;
	ModuleParticleImporter* particle_importer; 
	ModuleShaderImporter* shader_importer;

private:

	Timer	ms_timer;
	Timer	fps_timer;
	Timer	module_ms_timer;
	float	dt;
	std::list<Module*> list_modules;
	LCG*	random = nullptr;
	
	//Variables needed to show ms and fps graphs
	int	num_fps;
	int	last_frame_ms;
	int	last_fps;
	int frames;
	int capped_ms;
	SDL_Cursor* cursor;
	EngineState state = EngineState::OnStop;

	SDL_Surface* cursor_add;

};

// Give App pointer access everywhere
extern Application* App;