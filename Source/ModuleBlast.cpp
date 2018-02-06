#include "ModuleBlast.h"
#include "Nvidia/Blast/Include/toolkit/NvBlastTkFramework.h"
#include "Nvidia/Blast/Include/extensions/physx/NvBlastExtPxAsset.h"
#include "Nvidia/Blast/Include/extensions/physx/NvBlastExtPxManager.h"
#include "Nvidia/Blast/Include/extensions/physx/NvBlastExtPxFamily.h"
#include "Nvidia/Blast/Include/extensions/shaders/NvBlastExtDamageShaders.h"
#include "BlastModel.h"
#include "ModulePhysics.h"
#include "Application.h"

#if _DEBUG
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtPhysXDEBUG_x86.lib")
#else
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtPhysX_x86.lib")
#endif

ModuleBlast::ModuleBlast(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Blast_Module";
	framework = nullptr;
	px_manager = nullptr;
}

ModuleBlast::~ModuleBlast()
{
	framework->release();
	px_manager->release();
}

bool ModuleBlast::Init(Data * editor_config)
{
	framework = NvBlastTkFrameworkCreate();
	px_manager = Nv::Blast::ExtPxManager::create(*App->physics->GetPhysXPhysics(), *framework);

	if (framework == nullptr || px_manager == nullptr)
	{
		CONSOLE_ERROR("Blast Module can't be initialized!");
	}

	default_material = App->physics->GetPhysXPhysics()->createMaterial(0.8, 0.7, 0.1);

	return true;
}

bool ModuleBlast::CleanUp()
{
	px_manager->release();
	framework->release();
	return true;
}

Nv::Blast::TkFramework * ModuleBlast::GetFramework() const
{
	return framework;
}

void ModuleBlast::CreateFamily(BlastModel* model)
{
	Nv::Blast::ExtPxFamilyDesc desc;
	desc.actorDesc = nullptr;
	desc.group = nullptr;
	desc.pxAsset = model->m_pxAsset;

	NvBlastExtMaterial* mat = new NvBlastExtMaterial();

	Nv::Blast::ExtPxFamily* family = px_manager->createFamily(desc);
	family->setMaterial(mat);
	family->getPxAsset().setAccelerator(model->dmg_accel);
	family->subscribe(*this);
	families.push_back(family);
	model->family = family;
}

void ModuleBlast::SpawnFamily(BlastModel* model)
{
	physx::PxScene* scene = App->physics->GetScene(0);

	Nv::Blast::ExtPxSpawnSettings spawn_settings = {
		scene,
		default_material,
		2000.f
	};
	model->family->spawn(physx::PxTransform(physx::PxVec3(0, 0, 0)), physx::PxVec3(1, 1, 1), spawn_settings);
}

void ModuleBlast::onActorCreated(Nv::Blast::ExtPxFamily & family, Nv::Blast::ExtPxActor & actor)
{
	int i = 0;
}

void ModuleBlast::onActorDestroyed(Nv::Blast::ExtPxFamily & family, Nv::Blast::ExtPxActor & actor)
{
	int i = 0;
}
