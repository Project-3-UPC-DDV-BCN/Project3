#include "BlastModel.h"
#include "Data.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include <ctime>

BlastModel::BlastModel()
{
	SetType(Resource::BlastMeshResource);
}

BlastModel::BlastModel(const BlastModel & model)
{
	chunks = model.chunks;
	actors = model.actors;
	interior_material = model.interior_material;

	m_pxAsset = model.m_pxAsset;
	family = model.family;
	dmg_accel = model.dmg_accel;
}

BlastModel::~BlastModel()
{
	for (GameObject* go : chunks)
	{
		RELEASE(go);
	}
	chunks.clear();
}

void BlastModel::Save(Data & data) const
{
	for (GameObject* go : chunks)
	{
		go->Save(data);
	}
	data.AddInt("GameObjectsCount", App->scene->saving_index);
	data.AddString("library_path", GetLibraryPath());
	data.AddString("assets_path", GetAssetsPath());
	data.AddString("model_name", GetName());
	data.AddUInt("UUID", GetUID());
	App->scene->saving_index = 0;
}

bool BlastModel::Load(Data & data)
{
	int gameObjectsCount = data.GetInt("GameObjectsCount");
	chunks.clear();
	chunks.resize(gameObjectsCount);
	for (int i = 0; i < gameObjectsCount; i++) {
		GameObject* go = new GameObject();
		data.EnterSection("GameObject_" + std::to_string(i));
		go->Load(data, true);
		data.LeaveSection();
		chunks[i] = go;
		//App->resources->AddGameObject(go);
		go->SetIsUsedInPrefab(true);
	}
	SetUID(data.GetUInt("UUID"));
	SetAssetsPath(data.GetString("assets_path"));
	SetLibraryPath(data.GetString("library_path"));
	SetName(data.GetString("model_name"));
	return true;
}

void BlastModel::CreateMeta() const
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

void BlastModel::LoadToMemory()
{
}

void BlastModel::UnloadFromMemory()
{
}

void BlastModel::AddActor(Nv::Blast::ExtPxActor * actor)
{
	actors.emplace(actor);
}

void BlastModel::DestroyActor(Nv::Blast::ExtPxActor * actor)
{
	actors.erase(actors.find(actor));
}


