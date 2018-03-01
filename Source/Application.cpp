#include "Application.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleResources.h"
#include "ModuleTime.h"
#include "ModuleFileSystem.h"
#include "ModuleMeshImporter.h"
#include "ModuleTextureImporter.h"
#include "ModulePrefabImporter.h"
#include "Data.h"
#include "TagsAndLayers.h"
#include "ModuleMaterialImporter.h"
#include "ModuleParticleImporter.h"
#include "ModuleScriptImporter.h"
#include "ModulePhysics.h"
#include "ModulePhysMatImporter.h"
#include "ModuleBlastMeshImporter.h"
#include "ModuleShaderImporter.h"
#include "ModuleFontImporter.h"
#include "ModuleBlast.h"


Application::Application()
{
	frames = 0;
	last_frame_ms = -1;
	last_fps = 0;
	num_fps = 0;
	capped_ms = 1000 / 60;

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);
	resources = new ModuleResources(this);
	time = new ModuleTime(this);
	file_system = new ModuleFileSystem(this);
	mesh_importer = new ModuleMeshImporter(this);
	texture_importer = new ModuleTextureImporter(this);
	prefab_importer = new ModulePrefabImporter(this);
	material_importer = new ModuleMaterialImporter(this);
	script_importer = new ModuleScriptImporter(this);
	physics = new ModulePhysics(this);
	phys_mats_importer = new ModulePhysMatImporter(this);
	blast_mesh_importer = new ModuleBlastMeshImporter(this);
	particle_importer = new ModuleParticleImporter(this); 
	shader_importer = new ModuleShaderImporter(this);
	font_importer = new ModuleFontImporter(this);
	blast = new ModuleBlast(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(file_system);
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	AddModule(renderer3D);
	AddModule(mesh_importer);
	AddModule(texture_importer);
	AddModule(prefab_importer);
	AddModule(material_importer);
	AddModule(script_importer);
	AddModule(phys_mats_importer);
	AddModule(shader_importer);
	AddModule(particle_importer); 
	AddModule(camera);
	AddModule(scene);
	AddModule(editor);
	AddModule(font_importer);

	//TIME
	AddModule(time);
	AddModule(physics);
	AddModule(blast);
	AddModule(blast_mesh_importer);

	AddModule(resources);

	random = new math::LCG();
	cursor = nullptr;
	cursor_add = SDL_LoadBMP(EDITOR_IMAGES_FOLDER"PlusArrow.bmp");

	tags_and_layers = new TagsAndLayers();
}

Application::~Application()
{
	window = nullptr;
	input = nullptr;
	audio = nullptr;
	scene = nullptr;
	renderer3D = nullptr;
	camera = nullptr;
	editor = nullptr;
	resources = nullptr;
	time = nullptr;
	tags_and_layers = nullptr;
	file_system = nullptr;
	mesh_importer = nullptr;
	texture_importer = nullptr;
	prefab_importer = nullptr;
	material_importer = nullptr;
	particle_importer = nullptr; 
	script_importer = nullptr;
	phys_mats_importer = nullptr;
	physics = nullptr;
	blast_mesh_importer = nullptr;
	shader_importer = nullptr;
	blast = nullptr;

	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); ++it)
	{
		RELEASE(*it);
	}
	list_modules.clear();
	//std::list<Module*>::iterator item = list_modules.begin();

	//while (item != list_modules.end())
	//{
	//	RELEASE(*item);
	//	++item;
	//}

	//list_modules.clear();

	RELEASE(random);
	SDL_FreeCursor(cursor);
}

bool Application::Init()
{
	bool ret = true;

	Data data;

	if (!data.LoadJSON(EDITOR_CONFIG_FILE))
	{
		CreateEngineData(&data);
	}

	// Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	if (data.EnterSection("Engine_Settings"))
	{
		while (item != list_modules.end() && ret == true)
		{
			ret = (*item)->Init(&data);
			++item;
		}
		data.LeaveSection();
	}

	// After all Init calls we call Start() in all modules
	CONSOLE_DEBUG("-------------- Application Start --------------");
	item = list_modules.begin();

	while (item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start();
		++item;
	}
	
	ms_timer.Start();
	fps_timer.Start();

	tags_and_layers->Load(&data);

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	frames++;
	num_fps++;

	if (fps_timer.Read() >= 1000)//in ms
	{
		fps_timer.Start();
		last_fps = num_fps;
		num_fps = 0;
	}

	last_frame_ms = ms_timer.Read();
	
	if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		SDL_Delay(capped_ms - last_frame_ms);
	}

	App->editor->AddData_Editor(last_frame_ms, last_fps);
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator item = list_modules.begin();
	
	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if((*item)->is_game)
			ret = (*item)->PreUpdate(time->GetGameDt());
		else
			ret = (*item)->PreUpdate(dt);

		++item;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->is_game)
			ret = (*item)->Update(time->GetGameDt());
		else
			ret = (*item)->Update(dt);

		++item;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->is_game)
			ret = (*item)->PostUpdate(time->GetGameDt());
		else
			ret = (*item)->PostUpdate(dt);

		++item;
	}
	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	Data data;
	CreateEngineData(&data);

	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == true; ++it)
	{
		ret = (*it)->CleanUp();
	}

	return ret;
}

LCG & Application::RandomNumber()
{
	return *random;
}

void Application::CapFPS(int max_fps)
{
	if (max_fps > 0) capped_ms = 1000 / max_fps;
		
	else capped_ms = 0;
		
}

void Application::SetCustomCursor(EnGineCursors cursor_type)
{
	switch (cursor_type)
	{
	case Application::ENGINE_CURSOR_ADD:
		cursor = SDL_CreateColorCursor(cursor_add,0,0);
		break;
	case Application::ENGINE_CURSOR_ARROW:
		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		break;
	case Application::ENGINE_CURSOR_IBEAM:
		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
		break;
	case Application::ENGINE_CURSOR_WAIT:
		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
		break;
	case Application::ENGINE_CURSOR_WAITARROW:
		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
		break;
	case Application::ENGINE_CURSOR_HAND:
		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
		break;
	case Application::ENGINE_CURSOR_NO:
		cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
		break;
	default:
		break;
	}
	
	SDL_SetCursor(cursor);
}

void Application::CreateEngineData(Data * data)
{
	data->CreateSection("Engine_Settings");
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); ++it)
	{
		(*it)->SaveData(data);
	}
	tags_and_layers->Save(data);
	data->CloseSection();
	data->SaveAsJSON(EDITOR_CONFIG_FILE);
}

void Application::UpdateStep()
{
	if (state != OnPause) return;
	update_status ret = UPDATE_CONTINUE;

	std::list<Module*>::iterator item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->is_game)
			ret = (*item)->PreUpdate(dt);

		++item;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->is_game)
			ret = (*item)->Update(dt);

		++item;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		if ((*item)->is_game)
			ret = (*item)->PostUpdate(dt);

		++item;
	}
}

void Application::Play()
{
	if (state == OnStop) {
		state = OnPlay;
		if (!App->file_system->DirectoryExist(TMP_FOLDER_PATH))
		{
			App->file_system->Create_Directory(TMP_FOLDER_PATH);
		}

		App->scene->SaveScene(TMP_FOLDER"tmp_scene");
		App->scene->saving_index = 0;
		App->scene->is_game = true;
		App->scene->InitScripts();
		App->scene->SetParticleSystemsState(); 
	}
}

void Application::Pause()
{
	if (state == OnPlay) {
		state = OnPause;
	}
}

void Application::UnPause()
{
	if (state == OnPause) {
		state = OnPlay;
	}
}

void Application::Stop()
{
	if (state == OnPlay || state == OnPause) {
		state = OnStop;
		App->scene->LoadScene(TMP_FOLDER"tmp_scene");
		App->scene->is_game = false;
	}
}

bool Application::IsPlaying()
{
	return state == OnPlay;
}

bool Application::IsPaused()
{
	return state == OnPause;
}

bool Application::IsStopped()
{
	return state == OnStop;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}