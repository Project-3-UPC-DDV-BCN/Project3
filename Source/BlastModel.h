#pragma once

#include "Resource.h"
#include <vector>
#include <set>

class GameObject;
class Material;
class NvBlastExtDamageAccelerator;

namespace Nv
{
	namespace Blast
	{
		class ExtPxAsset;
		class ExtPxFamily;
		class ExtPxActor;
	}
}

class BlastModel :
	public Resource
{
public:
	BlastModel();
	~BlastModel();

	void CleanUp();

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

	void AddActor(Nv::Blast::ExtPxActor* actor);
	void DestroyActor(Nv::Blast::ExtPxActor* actor);

public:
	GameObject* root;
	std::vector<GameObject*> chunks;
	std::set<Nv::Blast::ExtPxActor*> actors;

	Nv::Blast::ExtPxAsset* m_pxAsset;
	Nv::Blast::ExtPxFamily* family;
	NvBlastExtDamageAccelerator* dmg_accel;
	bool family_created;
};

