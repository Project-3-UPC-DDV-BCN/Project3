#pragma once
#include "Module.h"
#include "Nvidia/PhysX/Include/PxPhysicsAPI.h"
#include "MathGeoLib/Geometry/AABB.h"
#include <map>

struct NvFlowContext;
struct NvFlowGrid;
class Mesh;

class ModulePhysics :
	public Module, public physx::PxSimulationEventCallback
{
public:

	ModulePhysics(Application* app, bool start_enabled = true, bool is_game = true);
	~ModulePhysics();

	bool Init(Data* editor_config = nullptr);
	update_status Update(float dt);
	bool CleanUp();

	void CreateScene();
	uint GetSceneCount() const;
	physx::PxScene* GetScene(uint index) const;
	physx::PxRigidDynamic* CreateDynamicRigidBody(physx::PxScene* scene = nullptr);
	physx::PxRigidStatic* CreateStaticRigidBody(physx::PxScene* scene = nullptr);
	void AddRigidBodyToScene(physx::PxRigidActor* body, physx::PxScene* scene);
	physx::PxMaterial* CreateMaterial(float static_friction, float dynamic_friction, float restitution);
	physx::PxShape* CreateShape(physx::PxRigidActor& body, physx::PxGeometry& geometry, physx::PxMaterial& mat);
	physx::PxTriangleMesh* CreateTriangleMesh(Mesh* mesh);
	physx::PxConvexMesh* CreateConvexMesh(Mesh* mesh);
	void SetCullingBox(AABB & box);
	physx::PxFixedJoint* CreateFixedJoint(physx::PxRigidActor* actor0, const physx::PxTransform& localFrame0, physx::PxRigidActor* actor1, const physx::PxTransform& localFrame1);
	physx::PxDistanceJoint* CreateDistanceJoint(physx::PxRigidActor* actor0, const physx::PxTransform& localFrame0, physx::PxRigidActor* actor1, const physx::PxTransform& localFrame1);
	physx::PxSphericalJoint* CreateSphericalJoint(physx::PxRigidActor* actor0, const physx::PxTransform& localFrame0, physx::PxRigidActor* actor1, const physx::PxTransform& localFrame1);
	physx::PxRevoluteJoint* CreateRevoluteJoint(physx::PxRigidActor* actor0, const physx::PxTransform& localFrame0, physx::PxRigidActor* actor1, const physx::PxTransform& localFrame1);
	physx::PxPrismaticJoint* CreatePrismaticJoint(physx::PxRigidActor* actor0, const physx::PxTransform& localFrame0, physx::PxRigidActor* actor1, const physx::PxTransform& localFrame1);
	physx::PxD6Joint* CreateD6Joint(physx::PxRigidActor* actor0, const physx::PxTransform& localFrame0, physx::PxRigidActor* actor1, const physx::PxTransform& localFrame1);

	physx::PxCooking* GetPhysXCooking() const;
	physx::PxPhysics* GetPhysXPhysics() const;
	physx::PxDefaultCpuDispatcher* GetPhysXCpuDispatcher() const;
	physx::PxFoundation* GetPhysXFoundation() const;
	void DrawColliders();

private:
	void CreateMainScene();
	void UpdateDynamicBody(physx::PxActor* actor);

	void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
	void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count);
	void onWake(physx::PxActor** actors, physx::PxU32 count);
	void onSleep(physx::PxActor** actors, physx::PxU32 count);
	void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
	void onAdvance(const physx::PxRigidBody*const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count);

private:
	physx::PxFoundation* physx_foundation;
	physx::PxPhysics* physx_physics;
	physx::PxCooking* cooking;
	physx::PxPvd* pvd;
	physx::PxCudaContextManager* cuda_context_manager;
	physx::PxDefaultCpuDispatcher* dispatcher;
	physx::PxScene* main_scene;

	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;

	std::map<physx::PxTriggerPair*, bool> trigger_stay_pairs;

	NvFlowContext* flow_context;
	NvFlowGrid* flow_grid;
};

