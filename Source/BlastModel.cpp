#include "BlastModel.h"
#include "Data.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleResources.h"
#include <ctime>
#include <NvBlastExtPxActor.h>
#include <NvBlastTkActor.h>
#include <NvBlastExtPxAsset.h>
#include <NvBlastExtPxFamily.h>
#include <NvBlastExtDamageShaders.h>

BlastModel::BlastModel()
{
	SetType(Resource::BlastMeshResource);
	root = nullptr;

	Nv::Blast::ExtPxAsset* m_pxAsset = nullptr;
	Nv::Blast::ExtPxFamily* family = nullptr;
	NvBlastExtDamageAccelerator* dmg_accel = nullptr;
	family_created = false;
}

BlastModel::~BlastModel()
{
	CleanUp();
}

void BlastModel::CleanUp()
{
	for (GameObject* go : chunks)
	{
		go = nullptr;
	}
	chunks.clear();
	for (Nv::Blast::ExtPxActor* actor : actors)
	{
		actor = nullptr;
	}
	actors.clear();
	if (family)
	{
		family = nullptr;
	}
	if (dmg_accel)
	{
		dmg_accel = nullptr;
	}
	family_created = false;
}

void BlastModel::Save(Data & data) const
{
	root->Save(data);
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
	chunks.resize(gameObjectsCount - 1);
	for (int i = 0; i < gameObjectsCount; i++) {
		GameObject* go = new GameObject();
		data.EnterSection("GameObject_" + std::to_string(i));
		go->Load(data, true);
		data.LeaveSection();
		go->SetIsUsedInPrefab(true);
		if (i == 0)
		{
			root = go;
		}
		else
		{
			chunks[i - 1] = go;
			go->SetParent(root);
		}
		
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


