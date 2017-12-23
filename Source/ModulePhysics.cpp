#include "ModulePhysics.h"

#pragma comment (lib, "PhysX/libx86/PhysX3CHECKED_x86.lib")
#pragma comment (lib, "PhysX/libx86/PhysX3CommonCHECKED_x86.lib")
#pragma comment (lib, "PhysX/libx86/PxFoundationCHECKED_x86.lib")

ModulePhysics::ModulePhysics(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Module Physics";
	physx_foundation = nullptr;
	physx_physics = nullptr;
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Init(Data * editor_config)
{
	bool ret = true;

	physx_foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!physx_foundation)
	{
		CONSOLE_DEBUG("Error. PxCreateFoundation failed!");
		ret = false;
	}
	else
	{
		physx_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *physx_foundation, physx::PxTolerancesScale(), true);
		if (!physx_physics)
		{
			CONSOLE_DEBUG("Error. PxCreatePhysics failed!");
			ret = false;
		}
	}

	return ret;
}

update_status ModulePhysics::Update(float dt)
{
	if (physx_physics && physx_physics->getNbScenes() > 0) {
		physx::PxScene* scene;
		physx_physics->getScenes(&scene, 1, 0);
		scene->simulate(dt);
	}
	return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	physx_physics->release();

	//foundation is last
	physx_foundation->release();
	return true;
}

void ModulePhysics::CreateScene()
{
	physx::PxSceneDesc scene_desc(physx_physics->getTolerancesScale());
	scene_desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	physx_physics->createScene(scene_desc);
}

physx::PxRigidDynamic* ModulePhysics::CreateDynamicRigidBody(physx::PxScene * scene)
{
	if (scene)
	{
		//Default rigidbody values
		physx::PxMaterial* mat = physx_physics->createMaterial(1,1,1);
		physx::PxTransform transform = physx::PxTransform(physx::PxIDENTITY());
		physx::PxBoxGeometry geometry = physx::PxBoxGeometry(0.5f, 0.5f, 0.5f);
		physx::PxRigidDynamic* body = physx::PxCreateDynamic(*physx_physics, transform, geometry, *mat, 1);
		scene->addActor(*body);
	}
}

physx::PxRigidStatic* ModulePhysics::CreateStaticRigidBody(physx::PxScene * scene)
{
	if (scene)
	{
		//Default rigidbody values
		physx::PxMaterial* mat = physx_physics->createMaterial(1, 1, 1);
		physx::PxTransform transform = physx::PxTransform(physx::PxIDENTITY());
		physx::PxBoxGeometry geometry = physx::PxBoxGeometry(0.5f, 0.5f, 0.5f);
		physx::PxRigidStatic* body = physx::PxCreateStatic(*physx_physics, transform, geometry, *mat);
		scene->addActor(*body);
	}
}

void ModulePhysics::AddRigidBodyToScene(physx::PxRigidActor* body, physx::PxScene* scene)
{
	if (scene)
	{
		scene->addActor(*body);
	}
}
