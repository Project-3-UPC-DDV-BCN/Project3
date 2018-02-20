#include "ModuleBlast.h"
#include <NvBlastTkFramework.h>
#include <NvBlastExtPxAsset.h>
#include <NvBlastExtPxManager.h>
#include <NvBlastExtPxFamily.h>
#include <NvBlastExtDamageShaders.h>
#include <NvBlastExtPxActor.h>
#include <NvBlastTkActor.h>
#include <NvBlastTkGroup.h>
#include <NvBlastPxCallbacks.h>
#include <NvBlastExtPxTask.h>
#include "BlastModel.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "ModuleScene.h"

#if _DEBUG
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtAssetUtilsDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtAuthoringDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtExporterDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtPhysXDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtPxSerializationDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtSerializationDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtShadersDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtStressDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastExtTkSerializationDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastGlobalsDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_debug/NvBlastTkDEBUG_x86.lib")

#else
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlast_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtAssetUtils_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtAuthoring_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtExporter_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtPhysX_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtPxSerialization_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtSerialization_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtShaders_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtStress_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastExtTkSerialization_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastGlobals_x86.lib")
#pragma comment (lib, "Nvidia/Blast/lib/lib_release/NvBlastTk_x86.lib")
#endif

ModuleBlast::ModuleBlast(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Blast_Module";
	framework = nullptr;
	px_manager = nullptr;
	damage_desc_buffer = new FixedBuffer(64 * 1024);
	damage_params_buffer = new FixedBuffer(1024);
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

	phys_task_manager = physx::PxTaskManager::createTaskManager(App->physics->GetPhysXFoundation()->getErrorCallback(), App->physics->GetPhysXCpuDispatcher(), 0);
	settings.damageFunctionData = this;
	impact_damage_manager = Nv::Blast::ExtImpactDamageManager::create(px_manager, settings);

	Nv::Blast::TkGroupDesc gdesc;
	gdesc.workerCount = phys_task_manager->getCpuDispatcher()->getWorkerCount();
	group = framework->createGroup(gdesc);

	task_manager = Nv::Blast::ExtGroupTaskManager::create(*phys_task_manager);
	task_manager->setGroup(group);

	return true;
}

update_status ModuleBlast::Update(float dt)
{
	BlastModel* model = families.begin()->second;
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		BlastModel* model = families.begin()->second;
		//NvBlastDamageProgram program;
		//program.graphShaderFunction = NvBlastExtFalloffGraphShader;
		//program.subgraphShaderFunction = NvBlastExtFalloffSubgraphShader;

		//NvBlastExtRadialDamageDesc desc =
		//{
		//	1000, { 0.2, 5, 0 }, 1.0, 2.0
		//};

		//const void* buffered_damage_desc = damage_desc_buffer->push(&desc, sizeof(desc));
		//NvBlastExtProgramParams params =
		//{
		//	buffered_damage_desc, model->family->getMaterial(), model->m_pxAsset->getAccelerator()
		//};

		//const void* buffered_program_params = damage_params_buffer->push(&params, sizeof(NvBlastExtProgramParams));
		//for (Nv::Blast::ExtPxActor* actor : model->actors)
		//{
		//	actor->getTkActor().damage(program, buffered_program_params);
		//}

		//if (model->actors[0]->getTkActor().isPending())
		//{
		//	//model->actors[0]->getTkActor().
		//}

	}

	for (Nv::Blast::ExtPxActor* actor : actors)
	{

	}

	task_manager->process();
	task_manager->wait();

	model->family->postSplitUpdate();

	return UPDATE_CONTINUE;
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

Nv::Blast::ExtImpactDamageManager * ModuleBlast::GetImpactManager() const
{
	return impact_damage_manager;
}

void ModuleBlast::CreateFamily(BlastModel* model)
{
	Nv::Blast::ExtPxFamilyDesc desc;
	desc.actorDesc = nullptr;
	desc.group = group;
	desc.pxAsset = model->m_pxAsset;

	NvBlastExtMaterial* mat = new NvBlastExtMaterial();

	Nv::Blast::ExtPxFamily* family = px_manager->createFamily(desc);
	family->setMaterial(mat);
	family->getPxAsset().setAccelerator(model->dmg_accel);
	family->subscribe(*this);
	families[family] = model;
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
	BlastModel* model = families[&family];
	const uint32_t* chunkIndices = actor.getChunkIndices();
	uint32_t chunkCount = actor.getChunkCount();
	for (uint32_t i = 0; i < chunkCount; i++)
	{
		uint32_t chunkIndex = chunkIndices[i];
		GameObject* go = model->chunks[chunkIndex];
		go->SetActive(true);
		App->scene->DuplicateGameObject(go);
	}
	//actor.getPhysXActor().userData = model->chunks[0];
	actors.emplace(&actor);
}

void ModuleBlast::onActorDestroyed(Nv::Blast::ExtPxFamily & family, Nv::Blast::ExtPxActor & actor)
{
	BlastModel* model = families[&family];
	const uint32_t* chunkIndices = actor.getChunkIndices();
	uint32_t chunkCount = actor.getChunkCount();
	for (uint32_t i = 0; i < chunkCount; i++)
	{
		uint32_t chunkIndex = chunkIndices[i];
		GameObject* go = model->chunks[chunkIndex];
		go->SetActive(false);
		App->scene->AddGameObjectToDestroy(go);
	}
	actors.erase(actors.find(&actor));
}

void ModuleBlast::ApplyDamage()
{
	//BlastModel* model = families.begin()->second;
	impact_damage_manager->applyDamage();
	//model->family->postSplitUpdate();
}
