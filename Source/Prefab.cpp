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
}

Prefab::~Prefab()
{
}

void Prefab::Save(Data & data) const
{
	data.AddInt("GameObjectsCount", App->scene->saving_index);
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("prefab_name", GetName());
	data.AddUInt("UUID", GetUID());
	App->scene->saving_index = 0;
}

bool Prefab::Load(Data & data)
{
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
