#pragma once
#include "Module.h"
#include "Nvidia/PhysX/Include/PxPhysicsAPI.h"

class ModulePhysics :
	public Module
{
public:
	ModulePhysics(Application* app, bool start_enabled = true, bool is_game = true);
	~ModulePhysics();

	bool Init(Data* editor_config = nullptr);
	update_status Update(float dt);
	bool CleanUp();

	void CreateScene();
	physx::PxRigidDynamic* CreateDynamicRigidBody(physx::PxScene* scene);
	physx::PxRigidStatic* CreateStaticRigidBody(physx::PxScene* scene);
	void AddRigidBodyToScene(physx::PxRigidActor* body, physx::PxScene* scene);

private:
	physx::PxFoundation* physx_foundation;
	physx::PxPhysics* physx_physics;

	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;
};

