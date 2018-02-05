#pragma once

#include "Module.h"
#include "Nvidia/Blast/Include/extensions/physx/NvBlastExtPxListener.h"
#include <vector>

namespace Nv
{
	namespace Blast
	{
		class TkFramework;
		class ExtPxManager;
		class ExtPxFamily;
	}
}

namespace physx
{
	class PxMaterial;
}

class BlastModel;

class ModuleBlast :
	public Module, public Nv::Blast::ExtPxListener
{
public:
	ModuleBlast(Application* app, bool start_enabled = true, bool is_game = false);
	~ModuleBlast();

	bool Init(Data* editor_config = nullptr);

	Nv::Blast::TkFramework* GetFramework() const;

	void CreateFamily(BlastModel* model);
	void SpawnFamily(BlastModel* model);

	void onActorCreated(Nv::Blast::ExtPxFamily& family, Nv::Blast::ExtPxActor& actor);
	void onActorDestroyed(Nv::Blast::ExtPxFamily& family, Nv::Blast::ExtPxActor& actor);

private:
	Nv::Blast::TkFramework* framework;
	Nv::Blast::ExtPxManager* px_manager;
	std::vector<Nv::Blast::ExtPxFamily*> families;
	physx::PxMaterial* default_material;
};

