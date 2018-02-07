#pragma once

#include "Resource.h"
#include <vector>

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

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

public:
	std::vector<GameObject*> chunks;
	std::vector<Nv::Blast::ExtPxActor*> actors;
	Material* interior_material;

	Nv::Blast::ExtPxAsset* m_pxAsset;
	Nv::Blast::ExtPxFamily* family;
	NvBlastExtDamageAccelerator* dmg_accel;
};

