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
#include "ComponentTransform.h"
#include "ComponentRigidBody.h"

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
	task_manager->process();
	task_manager->wait();

	if (App->IsPlaying())
	{

		//if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		//{
		//	BlastModel* model = families.begin()->second;
		//	NvBlastDamageProgram program;
		//	program.graphShaderFunction = NvBlastExtFalloffGraphShader;
		//	program.subgraphShaderFunction = NvBlastExtFalloffSubgraphShader;

		//	NvBlastExtRadialDamageDesc desc =
		//	{
		//		1, { 0.2, 0, 0 }, 1.0, 2.0
		//	};

		//	const void* buffered_damage_desc = damage_desc_buffer->push(&desc, sizeof(desc));
		//	NvBlastExtProgramParams params =
		//	{
		//		buffered_damage_desc, model->family->getMaterial(), model->m_pxAsset->getAccelerator()
		//	};

		//	const void* buffered_program_params = damage_params_buffer->push(&params, sizeof(NvBlastExtProgramParams));
		//	for (Nv::Blast::ExtPxActor* actor : model->actors)
		//	{
		//		actor->getTkActor().damage(program, buffered_program_params);
		//	}

		//	//if (model->actors.begin()->getTkActor().isPending())
		//	//{
		//	//	//model->actors[0]->getTkActor().
		//	//}

		//	
		//}

		for (std::map<Nv::Blast::ExtPxFamily*, BlastModel*>::iterator it = families.begin(); it != families.end(); it++)
		{
			for (Nv::Blast::ExtPxActor* actor : it->second->actors)
			{
				const Nv::Blast::ExtPxChunk* chunks = it->first->getPxAsset().getChunks();
				const Nv::Blast::ExtPxSubchunk* subChunks = it->first->getPxAsset().getSubchunks();
				const uint32_t* chunkIndices = actor->getChunkIndices();
				uint32_t chunkCount = actor->getChunkCount();
				for (uint32_t i = 0; i < chunkCount; i++)
				{
					uint32_t chunkIndex = chunkIndices[i];
					GameObject* go = it->second->chunks[chunkIndex];
					ComponentTransform* transform = (ComponentTransform*)go->GetComponent(Component::CompTransform);
					physx::PxTransform phys_transform = actor->getPhysXActor().getGlobalPose() * subChunks[chunks[chunkIndex].firstSubchunkIndex].transform;
					float3 pos(phys_transform.p.x, phys_transform.p.y, phys_transform.p.z);
					Quat quat(phys_transform.q.x, phys_transform.q.y, phys_transform.q.z, phys_transform.q.w);
					float3 rot = quat.ToEulerXYZ() * RADTODEG;
					ComponentTransform* parent_transform = (ComponentTransform*)it->second->root->GetComponent(Component::CompTransform);
					pos -= parent_transform->GetGlobalPosition();
					transform->SetPosition(pos);
					transform->SetRotation(rot);

					if (it->second->actors.size() > 1)
					{
						physx::PxTransform phys_transform = actor->getPhysXActor().getGlobalPose() * subChunks[chunks[chunkIndex].firstSubchunkIndex].transform;
						physx::PxVec3 dr = phys_transform.transform(actor->getPhysXActor().getCMassLocalPose()).p; //rigid_body->getGlobalPose().transform(rigid_body->getCMassLocalPose()).p/* - m_worldPos*/;
						float distance = dr.magnitude();
						float factor = physx::PxClamp(1.0f - (distance * distance) / (1 * 1), 0.0f, 1.0f);
						float impulse = factor * 100/* * 1000.0f*/;
						physx::PxVec3 vel = dr.getNormalized() * impulse / actor->getPhysXActor().getMass();
						actor->getPhysXActor().setLinearVelocity(actor->getPhysXActor().getLinearVelocity() + vel);
					}
				}
			}
			it->first->postSplitUpdate();
		}
	}

	task_manager->process();
	task_manager->wait();

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
	model->family_created = true;
}

void ModuleBlast::SpawnFamily(BlastModel* model, bool loaded_from_scene)
{
	physx::PxScene* scene = App->physics->GetScene(0);

	Nv::Blast::ExtPxSpawnSettings spawn_settings = {
		scene,
		default_material,
		2000.f
	};
	ComponentTransform* transform = (ComponentTransform*)model->root->GetComponent(Component::CompTransform);
	float3 pos = transform->GetGlobalPosition();
	model->family->spawn(physx::PxTransform(physx::PxVec3(pos.x, pos.y, pos.z)), physx::PxVec3(1, 1, 1), spawn_settings);
	if (!loaded_from_scene)
	{
		App->scene->AddGameObjectToScene(model->root);
		for (GameObject* go : model->root->childs)
		{
			App->scene->AddGameObjectToScene(go);
		}
	}
	
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
		/*ComponentRigidBody* rb = nullptr;
		if (go->GetComponent(Component::CompRigidBody) == nullptr)
		{
			rb = (ComponentRigidBody*)go->AddComponent(Component::CompRigidBody);
		}
		physx::PxRigidDynamic* rigid_body = &actor.getPhysXActor();*/
		if (chunkIndex != 0)
		{
			//const Nv::Blast::ExtPxChunk* chunks = family.getPxAsset().getChunks();
			//const Nv::Blast::ExtPxSubchunk* subChunks = family.getPxAsset().getSubchunks();
			//physx::PxTransform phys_transform = actor.getPhysXActor().getGlobalPose() * subChunks[chunks[chunkIndex].firstSubchunkIndex].transform;
			///*ComponentTransform* transform = (ComponentTransform*)go->GetComponent(Component::CompTransform);
			//float3 pos = transform->GetGlobalPosition();*/
			//physx::PxVec3 dr = phys_transform.transform(actor.getPhysXActor().getCMassLocalPose()).p; //rigid_body->getGlobalPose().transform(rigid_body->getCMassLocalPose()).p/* - m_worldPos*/;
			//float distance = dr.magnitude();
			//float factor = physx::PxClamp(1.0f - (distance * distance) / (1 * 1), 0.0f, 1.0f);
			//float impulse = factor * 100/* * 1000.0f*/;
			//physx::PxVec3 vel = dr.getNormalized() * impulse / actor.getPhysXActor().getMass();
			//actor.getPhysXActor().setLinearVelocity(actor.getPhysXActor().getLinearVelocity() + vel);
		}
		else
		{
			App->physics->AddActorToList(&actor.getPhysXActor(), go);
		}
		/*if (rb)
		{
			rb->SetNewRigidBody(rigid_body);
		}*/
	}
	model->AddActor(&actor);
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
		if (chunkIndex == 0)
		{
			App->physics->RemoveActorFromList(&actor.getPhysXActor());
		}
	}
	model->DestroyActor(&actor);
}

void ModuleBlast::ApplyDamage()
{
	impact_damage_manager->applyDamage();
}

void ModuleBlast::CleanFamilies()
{
	for (std::map<Nv::Blast::ExtPxFamily*, BlastModel*>::iterator it = families.begin(); it != families.end(); it++)
	{
		it->first->unsubscribe(*this);
		it->first->despawn();
		it->first->release();
		it->second->dmg_accel->release();
		it->second->family_created = false;
		RELEASE(it->second);
	}
	families.clear();
}

void ModuleBlast::CleanFamily(Nv::Blast::ExtPxFamily* family)
{
	if (families.find(family) != families.end())
	{
		family->unsubscribe(*this);
		family->despawn();
		family->release();
		families.erase(family);
	}
}
