#include "ModulePhysics.h"
#include "Mesh.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "MathGeoLib/Quat.h"
#include "MathGeoLib/float3.h"
#include "ComponentCollider.h"
#include "OpenGL.h"
#include "Application.h"
#include "ModuleTime.h"
#include "Primitive.h"
#include "ModuleBlast.h"
#include "DebugDraw.h"
#include "ModuleRenderer3D.h"

#if _DEBUG
#pragma comment (lib, "Nvidia/PhysX/lib/lib_debug/PhysX3DEBUG_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_debug/PhysX3CommonDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_debug/PxFoundationDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_debug/PhysX3ExtensionsDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_debug/PhysX3CookingDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_debug/PxPvdSDKDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_debug/PxTaskDEBUG_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_debug/PsFastXmlDEBUG_x86.lib")
#else
#pragma comment (lib, "Nvidia/PhysX/lib/lib_release/PhysX3_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_release/PhysX3Common_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_release/PxFoundation_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_release/PhysX3Cooking_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_release/PhysX3Extensions_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_release/PxPvdSDK_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_release/PxTask_x86.lib")
#pragma comment (lib, "Nvidia/PhysX/lib/lib_release/PsFastXml_x86.lib")
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled, bool is_game) : Module(app, start_enabled, is_game)
{
	name = "Module Physics";
	physx_foundation = nullptr;
	physx_physics = nullptr;
	cooking = nullptr;
	pvd = nullptr;
	cuda_context_manager = nullptr;
	dispatcher = nullptr;
	flow_context = nullptr;

	physx_foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!physx_foundation)
	{
		CONSOLE_DEBUG("Error. PxCreateFoundation failed!");
	}
	else
	{
		/*pvd = physx::PxCreatePvd(*physx_foundation);
		if (!pvd)
		{
		CONSOLE_DEBUG("Error. PxCreatePvd failed!");
		ret = false;
		}
		else
		{*/
		physx::PxTolerancesScale scale;
		physx_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *physx_foundation, scale, false/*, pvd*/);
		if (!physx_physics)
		{
			CONSOLE_DEBUG("Error. PxCreatePhysics failed!");
		}
		else
		{
			physx::PxCookingParams cooking_params(scale);
			cooking_params.buildGPUData = true;
			cooking = PxCreateCooking(PX_PHYSICS_VERSION, *physx_foundation, cooking_params);
			if (!cooking)
			{
				CONSOLE_DEBUG("Error. PxCreateCooking failed!");
			}
			else
			{
				//if (!PxInitExtensions(*physx_physics/*, pvd*/))
				//{
				//	CONSOLE_DEBUG("Error. PxInitExtensions failed!");
				//	ret = false;
				//}
				//else
				//{
				CreateMainScene();
				//}
			}
		}
		//}
	}

	/*NvFlowContextDescD3D11 desc;
	desc.device = createdevice;
	flow_context = NvFlowCreateContextD3D11(NV_FLOW_VERSION, )
	flow_grid = NvFlowCreateGrid();*/
}

ModulePhysics::~ModulePhysics()
{

}

bool ModulePhysics::Init(Data * editor_config)
{
	bool ret = true;

	return ret;
}

update_status ModulePhysics::Update(float dt)
{
	if (App->IsPlaying()/* || App->IsPaused()*/)
	{
		if (physx_physics && physx_physics->getNbScenes() > 0) {
			int scene_mum = physx_physics->getNbScenes();
			for (int i = 0; i < scene_mum; i++)
			{
				physx::PxScene* scene;
				physx_physics->getScenes(&scene, 1, i);
				scene->simulate(dt);
				App->blast->ApplyDamage();
				scene->fetchResults(true);
				//App->blast->ProcessGroup();

				physx::PxU32 active_actors_num;
				physx::PxActor** active_actors = scene->getActiveActors(active_actors_num);

				for (physx::PxU32 i = 0; i < active_actors_num; ++i)
				{
					physx::PxActorType::Enum type = active_actors[i]->getType();
					switch (type)
					{
					case physx::PxActorType::eRIGID_STATIC:
						break;
					case physx::PxActorType::eRIGID_DYNAMIC:
						UpdateDynamicBody(active_actors[i]);
						break;
					case physx::PxActorType::ePARTICLE_SYSTEM:
						break;
					case physx::PxActorType::ePARTICLE_FLUID:
						break;
					case physx::PxActorType::eARTICULATION_LINK:
						break;
					case physx::PxActorType::eCLOTH:
						break;
					case physx::PxActorType::eACTOR_COUNT:
						break;
					case physx::PxActorType::eACTOR_FORCE_DWORD:
						break;
					default:
						break;
					}
				}
			}
		}
	}
	
	//DrawColliders();
	
	return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	if (physx_physics && physx_physics->getNbScenes() > 0) {
		int scene_mum = physx_physics->getNbScenes();
		for (int i = 0; i < scene_mum; i++)
		{
			physx::PxScene* scene;
			physx_physics->getScenes(&scene, 1, i);
			scene->release();
		}
	}
	dispatcher->release();
	cuda_context_manager->release();
	cooking->release();
	//pvd->release();
	physx_physics->release();

	//foundation is last
	physx_foundation->release();
	return true;
}

uint ModulePhysics::GetSceneCount() const
{
	return physx_physics->getNbScenes();
}

physx::PxScene * ModulePhysics::GetScene(uint index) const
{
	physx::PxScene* scene;
	physx_physics->getScenes(&scene, 1, index);
	return scene;
}

physx::PxRigidDynamic* ModulePhysics::CreateDynamicRigidBody(physx::PxScene * scene)
{
	physx::PxRigidDynamic* ret = nullptr;

	//Default rigidbody values
	physx::PxTransform transform = physx::PxTransform(physx::PxIDENTITY());
	ret = physx_physics->createRigidDynamic(transform);

	return ret;
}

physx::PxRigidStatic* ModulePhysics::CreateStaticRigidBody(physx::PxScene * scene)
{
	physx::PxRigidStatic* ret = nullptr;

	//Default rigidbody values
	physx::PxTransform transform = physx::PxTransform(physx::PxIDENTITY());
	ret = physx_physics->createRigidStatic(transform);
	/*if (scene != nullptr)
	{
		scene->addActor(*ret);
	}
	else
	{
		main_scene->addActor(*ret);
	}*/
	
	return ret;
}

void ModulePhysics::AddRigidBodyToScene(physx::PxRigidActor* body, physx::PxScene* scene)
{
	if (body)
	{
		if (scene)
		{
			if (body->getScene() != scene)
			{
				scene->addActor(*body);
			}
		}
		else
		{
			if (body->getScene() != main_scene)
			{
				main_scene->addActor(*body);
			}
		}
		
	}
}

void ModulePhysics::RemoveRigidBodyFromScene(physx::PxRigidActor * body, physx::PxScene * scene)
{
	if (body)
	{
		if (scene)
		{
			/*physx::PxU32 dynamic_actors_num = scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);
			physx::PxActor** dynamic_actors;
			scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC, dynamic_actors, dynamic_actors_num);
			physx::PxActor* actor = nullptr;

			if (!actor)
			{
				for (physx::PxU32 i = 0; i < dynamic_actors_num; ++i)
				{
					if (dynamic_actors[i] == body)
					{
						actor == body;
						break;
					}
				}
			}

			if (!actor)
			{
				physx::PxU32 static_actors_num = scene->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC);
				physx::PxActor** static_actors;
				for (physx::PxU32 i = 0; i < static_actors_num; ++i)
				{
					if (static_actors[i] == body)
					{
						actor == body;
						break;
					}
				}
			}*/

			scene->removeActor(*body);
		}
		else
		{
			/*physx::PxU32 dynamic_actors_num = main_scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);
			physx::PxActor** dynamic_actors = new physx::PxActor*[dynamic_actors_num];
			main_scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC, dynamic_actors, dynamic_actors_num);
			physx::PxActor* actor = nullptr;

			if (!actor)
			{
				for (physx::PxU32 i = 0; i < dynamic_actors_num; ++i)
				{
					if (dynamic_actors[i] == body)
					{
						actor == body;
						break;
					}
				}
			}

			if (!actor)
			{
				physx::PxU32 static_actors_num = main_scene->getNbActors(physx::PxActorTypeFlag::eRIGID_STATIC);
				physx::PxActor** static_actors;
				for (physx::PxU32 i = 0; i < static_actors_num; ++i)
				{
					if (static_actors[i] == body)
					{
						actor == body;
						break;
					}
				}
			}*/

			main_scene->removeActor(*body);
		}
		RemoveActorFromList(body);
	}
}

physx::PxMaterial * ModulePhysics::CreateMaterial(float static_friction, float dynamic_friction, float restitution)
{
	return physx_physics->createMaterial(static_friction, dynamic_friction, restitution);
}

physx::PxShape * ModulePhysics::CreateShape(physx::PxRigidActor& body, physx::PxGeometry& geometry, physx::PxMaterial& mat)
{
	//physx::PxShape* shape = physx_physics->createShape(geometry, mat, true);
	//body.attachShape(*shape);
	//shape->release();
	return physx::PxRigidActorExt::createExclusiveShape(body, geometry, mat);
	//return shape;
}

physx::PxTriangleMesh * ModulePhysics::CreateTriangleMesh(Mesh * mesh)
{
	physx::PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = mesh->num_vertices;
	meshDesc.points.stride = sizeof(physx::PxVec3);
	meshDesc.points.data = mesh->vertices;

	meshDesc.triangles.count = mesh->num_indices;
	meshDesc.triangles.stride = 3 * sizeof(physx::PxU32);
	meshDesc.triangles.data = mesh->indices;

	physx::PxDefaultMemoryOutputStream writeBuffer;
	physx::PxTriangleMeshCookingResult::Enum result;
	physx::PxCookingParams params = cooking->getParams();
	params.buildGPUData = true;
	cooking->setParams(params);
	bool status = cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status)
		return nullptr;

	physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	return physx_physics->createTriangleMesh(readBuffer);
}

physx::PxConvexMesh * ModulePhysics::CreateConvexMesh(Mesh * mesh)
{
	physx::PxConvexMeshDesc convexDesc;
	convexDesc.points.count = mesh->num_vertices;
	convexDesc.points.stride = sizeof(physx::PxVec3);
	convexDesc.points.data = mesh->vertices;
	convexDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	physx::PxDefaultMemoryOutputStream buf;
	physx::PxConvexMeshCookingResult::Enum result;
	if (!cooking->cookConvexMesh(convexDesc, buf, &result))
		return NULL;
	physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
	return physx_physics->createConvexMesh(input);
}

void ModulePhysics::SetCullingBox(AABB & box)
{
	if (physx_physics && physx_physics->getNbScenes() > 0) {
		int scene_mum = physx_physics->getNbScenes();
		for (int i = 0; i < scene_mum; i++)
		{
			physx::PxScene* scene;
			physx_physics->getScenes(&scene, 1, i);
			scene->setVisualizationCullingBox(physx::PxBounds3(physx::PxVec3(box.MinX(), box.MinY(), box.MinZ()), physx::PxVec3(box.MaxX(), box.MaxY(), box.MaxZ())));
		}
	}
}

physx::PxFixedJoint* ModulePhysics::CreateFixedJoint(physx::PxRigidActor * actor0, const physx::PxTransform & localFrame0, physx::PxRigidActor * actor1, const physx::PxTransform & localFrame1)
{
	return physx::PxFixedJointCreate(*physx_physics, actor0, localFrame0, actor1, localFrame1);
}

physx::PxDistanceJoint* ModulePhysics::CreateDistanceJoint(physx::PxRigidActor * actor0, const physx::PxTransform & localFrame0, physx::PxRigidActor * actor1, const physx::PxTransform & localFrame1)
{
	return physx::PxDistanceJointCreate(*physx_physics, actor0, localFrame0, actor1, localFrame1);
}

physx::PxSphericalJoint* ModulePhysics::CreateSphericalJoint(physx::PxRigidActor * actor0, const physx::PxTransform & localFrame0, physx::PxRigidActor * actor1, const physx::PxTransform & localFrame1)
{
	return physx::PxSphericalJointCreate(*physx_physics, actor0, localFrame0, actor1, localFrame1);
}

physx::PxRevoluteJoint* ModulePhysics::CreateRevoluteJoint(physx::PxRigidActor * actor0, const physx::PxTransform & localFrame0, physx::PxRigidActor * actor1, const physx::PxTransform & localFrame1)
{
	return physx::PxRevoluteJointCreate(*physx_physics, actor0, localFrame0, actor1, localFrame1);
}

physx::PxPrismaticJoint* ModulePhysics::CreatePrismaticJoint(physx::PxRigidActor * actor0, const physx::PxTransform & localFrame0, physx::PxRigidActor * actor1, const physx::PxTransform & localFrame1)
{
	return physx::PxPrismaticJointCreate(*physx_physics, actor0, localFrame0, actor1, localFrame1);
}

physx::PxD6Joint* ModulePhysics::CreateD6Joint(physx::PxRigidActor * actor0, const physx::PxTransform & localFrame0, physx::PxRigidActor * actor1, const physx::PxTransform & localFrame1)
{
	return physx::PxD6JointCreate(*physx_physics, actor0, localFrame0, actor1, localFrame1);
}

static physx::PxFilterFlags CCDShader(
	physx::PxFilterObjectAttributes attributes0,
	physx::PxFilterData filterData0,
	physx::PxFilterObjectAttributes attributes1,
	physx::PxFilterData filterData1,
	physx::PxPairFlags& pairFlags,
	const void* constantBlock,
	physx::PxU32 constantBlockSize)
{
	pairFlags = physx::PxPairFlag::eSOLVE_CONTACT;
	pairFlags |= physx::PxPairFlag::eDETECT_DISCRETE_CONTACT;
	pairFlags |= physx::PxPairFlag::eDETECT_CCD_CONTACT;
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
	pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
	
	// let triggers through
	if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}

	return physx::PxFilterFlag::eDEFAULT;
}

void ModulePhysics::CreateMainScene()
{
	physx::PxCudaContextManagerDesc cudaContextManagerDesc;
	cuda_context_manager = PxCreateCudaContextManager(*physx_foundation, cudaContextManagerDesc);
	
	physx::PxSceneDesc sceneDesc(physx_physics->getTolerancesScale());
	//sceneDesc.gravity = physx::PxVec3(0.0f, 0.0f, 0.0f);
	dispatcher = physx::PxDefaultCpuDispatcherCreate(4);
	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = Nv::Blast::ExtImpactDamageManager::FilterShader; //CCDShader;
	sceneDesc.simulationEventCallback = this;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS | physx::PxSceneFlag::eENABLE_PCM | physx::PxSceneFlag::eENABLE_STABILIZATION;

	if (cuda_context_manager)
	{
		sceneDesc.gpuDispatcher = cuda_context_manager->getGpuDispatcher();
		
		sceneDesc.flags |= physx::PxSceneFlag::eENABLE_GPU_DYNAMICS;
		sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eGPU;

		sceneDesc.gpuDynamicsConfig.constraintBufferCapacity *= 4;
		sceneDesc.gpuDynamicsConfig.contactBufferCapacity *= 4;
		sceneDesc.gpuDynamicsConfig.contactStreamSize *= 4;
		sceneDesc.gpuDynamicsConfig.forceStreamCapacity *= 4;
		sceneDesc.gpuDynamicsConfig.foundLostPairsCapacity *= 4;
		sceneDesc.gpuDynamicsConfig.patchStreamSize *= 4;
		sceneDesc.gpuDynamicsConfig.tempBufferCapacity *= 4;
	}
	else
	{
		CONSOLE_WARNING("Can't use gpu physics. Not gpu physics are used.");
		sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eMBP;
	}
	
	main_scene = physx_physics->createScene(sceneDesc);
	main_scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	main_scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);
}

void ModulePhysics::CreateScene()
{
	physx::PxSceneDesc sceneDesc(physx_physics->getTolerancesScale());
	//sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = CCDShader;
	//sceneDesc.gpuDispatcher = cuda_context_manager->getGpuDispatcher();

	sceneDesc.flags |= /*physx::PxSceneFlag::eENABLE_GPU_DYNAMICS |*/ physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS | physx::PxSceneFlag::eENABLE_PCM;
	sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eMBP /*physx::PxBroadPhaseType::eGPU*/;

	physx::PxScene* scene = physx_physics->createScene(sceneDesc);
	scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 2.0f);
}

void ModulePhysics::UpdateDynamicBody(physx::PxActor * actor)
{
	physx::PxRigidActor* rigid_actor = static_cast<physx::PxRigidActor*>(actor);
	if (rigid_actor->userData == nullptr)
	{
		if (physics_objects.find(rigid_actor) != physics_objects.end())
		{
			GameObject* go = physics_objects[rigid_actor];
			if (go == nullptr) return;
			physx::PxTransform phys_transform = rigid_actor->getGlobalPose();
			ComponentTransform* transform = (ComponentTransform*)go->GetComponent(Component::CompTransform);
			float3 position(phys_transform.p.x, phys_transform.p.y, phys_transform.p.z);
			Quat rot_quat(phys_transform.q.x, phys_transform.q.y, phys_transform.q.z, phys_transform.q.w);
			float3 rotation = rot_quat.ToEulerXYZ() * RADTODEG;
			transform->SetPosition(position);
			transform->SetRotation(rotation);
		}
	}
}

void ModulePhysics::onTrigger(physx::PxTriggerPair * pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; i++)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER |
			physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;
		
		if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			//Call OnTriggerEnter in script module

			trigger_stay_pairs[&pairs[i]] = true;
		}
		else if (pairs[i].status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
		{ 
			//Call OnTriggerExit in script module
			trigger_stay_pairs.erase(&pairs[i]);
		}
		else if (trigger_stay_pairs[&pairs[i]] == true)
		{
			//Call OnTriggerStay in script module
		}
	}
}

void ModulePhysics::onConstraintBreak(physx::PxConstraintInfo * constraints, physx::PxU32 count)
{
}

void ModulePhysics::onWake(physx::PxActor ** actors, physx::PxU32 count)
{
}

void ModulePhysics::onSleep(physx::PxActor ** actors, physx::PxU32 count)
{
}

void ModulePhysics::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	for (physx::PxU32 i = 0; i < nbPairs; i++)
	{
		const physx::PxContactPair& cp = pairs[i];

		if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			//Call OnCollisionEnter in script module
			for (std::map<physx::PxRigidActor*, GameObject*>::iterator it = physics_objects.begin(); it != physics_objects.end(); it++)
			{
				if (pairHeader.actors[0] == it->first)
				{
					//it->second->OnCollisionEnter(pairHeader.actors[1]);
				}
			}
		}
		else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
		{
			//Call OnCollisionStay in script module
			for (std::map<physx::PxRigidActor*, GameObject*>::iterator it = physics_objects.begin(); it != physics_objects.end(); it++)
			{
				if (pairHeader.actors[0] == it->first)
				{
					//it->second->OnCollisionStay(pairHeader.actors[1]);
				}
			}
		}
		else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			//Call OnCollisionExit in script module
			for (std::map<physx::PxRigidActor*, GameObject*>::iterator it = physics_objects.begin(); it != physics_objects.end(); it++)
			{
				if (pairHeader.actors[0] == it->first)
				{
					//it->second->OnCollisionExit(pairHeader.actors[1]);
				}
			}
		}
	}

	Nv::Blast::ExtImpactDamageManager* impact_manager = App->blast->GetImpactManager();
	impact_manager->onContact(pairHeader, pairs, nbPairs);
}

void ModulePhysics::onAdvance(const physx::PxRigidBody * const * bodyBuffer, const physx::PxTransform * poseBuffer, const physx::PxU32 count)
{
}

physx::PxCooking * ModulePhysics::GetPhysXCooking() const
{
	return cooking;
}

physx::PxPhysics * ModulePhysics::GetPhysXPhysics() const
{
	return physx_physics;
}

physx::PxDefaultCpuDispatcher * ModulePhysics::GetPhysXCpuDispatcher() const
{
	return dispatcher;
}

physx::PxFoundation * ModulePhysics::GetPhysXFoundation() const
{
	return physx_foundation;
}

void ModulePhysics::DrawColliders()
{
	if (physx_physics && physx_physics->getNbScenes() > 0) {
		int scene_mum = physx_physics->getNbScenes();
		for (int i = 0; i < scene_mum; i++)
		{
			physx::PxScene* scene;
			physx_physics->getScenes(&scene, 1, i);
			if (!App->IsPlaying())
			{
				physx::PxU32 result;
				scene->simulate(0.00000000000000000000000000001); //enough to draw shapes but not to move bodies/colliders
				scene->fetchResults(true, &result);
				if (result != 0)
				{
					CONSOLE_LOG("Simulation error: %zu", result);
				}
			}

			const physx::PxRenderBuffer& rb = scene->getRenderBuffer();

			for (uint i = 0; i < rb.getNbLines(); i++)
			{
				const physx::PxDebugLine& line = rb.getLines()[i];
				App->renderer3D->debug_draw->Line({ line.pos0.x, line.pos0.y, line.pos0.z }, { line.pos1.x, line.pos1.y, line.pos1.z });
			}
			
			/*glBegin(GL_TRIANGLES);
			for (uint i = 0; i < rb.getNbTriangles(); i++)
			{
				const physx::PxDebugTriangle& triangles = rb.getTriangles()[i];

				glVertex3f(triangles.pos0.x, triangles.pos0.y, triangles.pos0.z);
				glVertex3f(triangles.pos1.x, triangles.pos1.y, triangles.pos1.z);
				glVertex3f(triangles.pos2.x, triangles.pos2.y, triangles.pos2.z);
			}
			glEnd();
			glBegin(GL_POINTS);
			for (uint i = 0; i < rb.getNbPoints(); i++)
			{
				const physx::PxDebugPoint& point = rb.getPoints()[i];

				glVertex3f(point.pos.x, point.pos.y, point.pos.z);
			}
			glEnd();
			glLineWidth(1.0f);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);*/
		}
	}
}

void ModulePhysics::AddActorToList(physx::PxRigidActor * body, GameObject * gameobject)
{
	if (gameobject == nullptr || body == nullptr) return;
	physics_objects.insert(std::pair<physx::PxRigidActor*, GameObject*>(body, gameobject));
}

void ModulePhysics::RemoveActorFromList(physx::PxRigidActor * body)
{
	if (body == nullptr) return;
	physics_objects.erase(body);
}

void ModulePhysics::CleanPhysScene()
{
	for (std::map<physx::PxRigidActor*, GameObject*>::iterator it = physics_objects.begin(); it != physics_objects.end(); it++)
	{
		main_scene->removeActor(*it->first);
	}
	physics_objects.clear();
}
