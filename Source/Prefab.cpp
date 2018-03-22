#include "Prefab.h"
#include "Data.h"
#include <ctime>
#include <vector>
#include "GameObject.h"
#include "ModuleResources.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"

Prefab::Prefab()
{
	SetType(Resource::PrefabResource);
	root_gameobject = nullptr;
}

Prefab::~Prefab()
{
}

void Prefab::AddGameObject(GameObject * game_object)
{
	prefab_gameobjects.push_back(game_object);
}

std::vector<GameObject*> Prefab::GetGameObjects() const
{
	return prefab_gameobjects;
}

void Prefab::SetRootGameObject(GameObject * root)
{
	root_gameobject = root;
}

GameObject* Prefab::GetRootGameObject() const
{
	return root_gameobject;
}

void Prefab::Save(Data & data) const
{
	root_gameobject->Save(data);
	data.AddInt("GameObjectsCount", App->scene->saving_index);
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("prefab_name", GetName());
	data.AddUInt("UUID", GetUID());
	App->scene->saving_index = 0;
}

bool Prefab::Load(Data & data)
{
	//std::list<GameObject*> created_objects;
	//int gameObjectsCount = data.GetInt("GameObjectsCount");
	//for (int i = 0; i < gameObjectsCount; i++) {
	//	GameObject* go = new GameObject();
	//	data.EnterSection("GameObject_" + std::to_string(i));
	//	go->Load(data, true);
	//	data.LeaveSection();
	//	AddGameObject(go);
	//	App->resources->AddGameObject(go);
	//	go->SetIsUsedInPrefab(true);
	//	if (i == 0) root_gameobject = go;
	//}

	SetUID(data.GetUInt("UUID"));
	SetAssetsPath(data.GetString("assets_path"));
	SetLibraryPath(data.GetString("library_path"));
	SetName(data.GetString("prefab_name"));
	return true;
}

void Prefab::CreateMeta() const
{
	time_t now = time(0);
	char* dt = ctime(&now);

	Data data;
	data.AddInt("Type", GetType());
	data.AddUInt("UUID", GetUID());
	data.AddString("Time_Created", dt);
	data.AddString("Library_path", GetLibraryPath());
	data.SaveAsMeta(GetAssetsPath());
}

void Prefab::LoadToMemory()
{
}

void Prefab::UnloadFromMemory()
{
}
