#include "ComponentRigidBody.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "MathGeoLib/Quat.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ComponentCollider.h"
#include "Nvidia/PhysX/Include/PxRigidDynamic.h"
#include "Nvidia/PhysX/Include/PxShape.h"

ComponentRigidBody::ComponentRigidBody(GameObject* attached_gameobject)
{
	SetActive(true);
	SetName("RigidBody");
	SetType(ComponentType::CompRigidBody);
	SetGameObject(attached_gameobject);
	rigidbody = App->physics->CreateDynamicRigidBody();
	//rigidbody->userData = attached_gameobject;

	float* matrix = attached_gameobject->GetOpenGLMatrix().ptr();
	physx::PxMat44 mat(matrix);
	physx::PxTransform phys_transform(mat);
	rigidbody->setGlobalPose(phys_transform);

	curr_rot = float3(0.f, 0.f, 0.f);

	SetInitValues();
}

ComponentRigidBody::~ComponentRigidBody()
{
	if (App->physics)
	{
		App->physics->RemoveRigidBodyFromScene(rigidbody, nullptr);
	}
	rigidbody = nullptr;
}

void ComponentRigidBody::SetInitValues()
{
	SetMass(1);
	SetUseGravity(false);
	SetKinematic(false);
	SetLinearDamping(0);
	SetAngularDamping(0.05f);
	SetLinearVelocity(float3(0, 0, 0));
	SetLinearDamping(0);
}

void ComponentRigidBody::SetMass(float new_mass)
{
	if (new_mass < 0) new_mass = 0;
	if (new_mass > PX_MAX_F32) new_mass = PX_MAX_F32;
	rigidbody->setMass(new_mass);
}

float ComponentRigidBody::GetMass() const
{
	return rigidbody->getMass();
}

void ComponentRigidBody::SetUseGravity(bool use)
{
	rigidbody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !use);
	use_gravity = use;
}

bool ComponentRigidBody::IsUsingGravity() const
{
	return use_gravity;
}

void ComponentRigidBody::SetKinematic(bool kinematic)
{
	std::vector<physx::PxShape*> shapes = GetShapes();
	
	bool can_change = true;

	if (!kinematic)
	{
		for (uint i = 0; i < shapes.size(); i++)
		{
			if (shapes[i]->getGeometryType() == physx::PxGeometryType::eTRIANGLEMESH)
			{
				ComponentCollider* collider = (ComponentCollider*)shapes[i]->userData;
				if (collider->IsActive())
				{
					can_change = false;
					CONSOLE_ERROR("RigidBody needs to be kinematic if GameObject have a Mesh Collider and it's active. \nIf you need to set the RigidBody as kinematic, remove/disable the Mesh Collider or set it convex");
				}
			}
		}
	}
	
	if (can_change)
	{
		rigidbody->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, kinematic);
		is_kinematic = kinematic;
	}
	
}

bool ComponentRigidBody::IsKinematic() const
{
	return is_kinematic;
}

void ComponentRigidBody::SetLinearVelocity(float3 linear_velocity)
{
	physx::PxVec3 velocity(linear_velocity.x, linear_velocity.y, linear_velocity.z);
	rigidbody->setLinearVelocity(velocity);
}

float3 ComponentRigidBody::GetLinearVelocity() const
{
	physx::PxVec3 phys_velocity = rigidbody->getLinearVelocity();
	float3 velocity(phys_velocity.x, phys_velocity.y, phys_velocity.z);
	return velocity;
}

void ComponentRigidBody::SetAngularVelocity(float3 angular_velocity)
{
	physx::PxVec3 phys_angular_velocity(angular_velocity.x, angular_velocity.y, angular_velocity.z);
	rigidbody->setAngularVelocity(phys_angular_velocity);
}

float3 ComponentRigidBody::GetAngularVelocity() const
{
	physx::PxVec3 phys_angular_velocity = rigidbody->getAngularVelocity();
	float3 angular_velocity(phys_angular_velocity.x, phys_angular_velocity.y, phys_angular_velocity.z);
	return angular_velocity;
}

void ComponentRigidBody::SetMaxAngularVelocity(float max_velocity)
{
	rigidbody->setMaxAngularVelocity(max_velocity);
}

float ComponentRigidBody::GetMaxAngularVelocity() const
{
	return rigidbody->getMaxAngularVelocity();
}

void ComponentRigidBody::SetLinearDamping(float linear_damping)
{
	rigidbody->setLinearDamping(linear_damping);
}

float ComponentRigidBody::GetLinearDamping() const
{
	return rigidbody->getLinearDamping();
}

void ComponentRigidBody::SetAngularDamping(float angular_damping)
{
	rigidbody->setAngularDamping(angular_damping);
}

float ComponentRigidBody::GetAngularDamping() const
{
	return rigidbody->getAngularDamping();
}

void ComponentRigidBody::AddForce(float3 force, int force_mode)
{
	physx::PxVec3 phys_force(force.x, force.y, force.z);
	if (force_mode < 0 || force_mode > 3)
	{
		CONSOLE_ERROR("Force Mode is not valid");
		return;
	}
	rigidbody->addForce(phys_force, (physx::PxForceMode::Enum)force_mode);
}

void ComponentRigidBody::ClearForce(int force_mode)
{
	if (force_mode < 0 || force_mode > 3)
	{
		CONSOLE_ERROR("Force Mode is not valid");
		return;
	}
	rigidbody->clearForce((physx::PxForceMode::Enum)force_mode);
}

void ComponentRigidBody::AddTorque(float3 torque, int force_mode)
{
	physx::PxVec3 phys_torque(torque.x, torque.y, torque.z);
	if (force_mode < 0 || force_mode > 3)
	{
		CONSOLE_ERROR("Force Mode is not valid");
		return;
	}
	rigidbody->addForce(phys_torque, (physx::PxForceMode::Enum)force_mode);
}

void ComponentRigidBody::ClearTorque(int force_mode)
{
	if (force_mode < 0 || force_mode > 3)
	{
		CONSOLE_ERROR("Force Mode is not valid");
		return;
	}
	rigidbody->clearTorque((physx::PxForceMode::Enum)force_mode);
}

bool ComponentRigidBody::IsSleeping() const
{
	return rigidbody->isSleeping();
}

void ComponentRigidBody::SetToSleep()
{
	rigidbody->putToSleep();
}

void ComponentRigidBody::WakeUp()
{
	rigidbody->wakeUp();
}

void ComponentRigidBody::SetPosition(float3 new_position)
{
	physx::PxVec3 position(new_position.x, new_position.y, new_position.z);
	physx::PxTransform transform(position, rigidbody->getGlobalPose().q);
	rigidbody->setGlobalPose(transform);
}

float3 ComponentRigidBody::GetPosition() const
{
	physx::PxTransform phys_position = rigidbody->getGlobalPose();
	float3 position(phys_position.p.x, phys_position.p.y, phys_position.p.z);
	return position;
}

void ComponentRigidBody::SetRotation(float3 new_rotation)
{
	/*Quat rot_q = math::Quat::FromEulerXYZ(curr_rot.x * DEGTORAD, curr_rot.y * DEGTORAD, curr_rot.z * DEGTORAD);
	float3 diff = new_rotation - curr_rot;

	Quat q = math::Quat::FromEulerXYZ(diff.x * DEGTORAD, diff.y * DEGTORAD, diff.z * DEGTORAD);

	Quat final_rot = rot_q * q;*/

	Quat final_rot = math::Quat::FromEulerXYZ(new_rotation.x * DEGTORAD, new_rotation.y * DEGTORAD, new_rotation.z * DEGTORAD);

	physx::PxQuat rotation(final_rot.x, final_rot.y, final_rot.z, final_rot.w);

	physx::PxTransform transform(rigidbody->getGlobalPose().p ,rotation);

	rigidbody->setGlobalPose(transform);
	curr_rot = new_rotation;

	/*float3 a = Quat(final_rot.x, final_rot.y, final_rot.z, final_rot.w).ToEulerXYZ() * RADTODEG;
	CONSOLE_LOG("RB Set rot: %.3f,%.3f,%.3f", a.x, a.y, a.z);

	Quat rot_q2 = math::Quat::FromEulerXYZ(new_rotation.x * DEGTORAD, new_rotation.y * DEGTORAD, new_rotation.z * DEGTORAD);
	float3 a2 = rot_q2.ToEulerXYZ() * RADTODEG;
	CONSOLE_LOG("RB Set rot2: %.3f,%.3f,%.3f", a2.x, a2.y, a2.z);*/
}

float3 ComponentRigidBody::GetRotation() const
{
	physx::PxTransform phys_rotation = rigidbody->getGlobalPose();
	Quat q(phys_rotation.q.x, phys_rotation.q.y, phys_rotation.q.z, phys_rotation.q.w);
	float3 rotation = q.ToEulerXYZ() * RADTODEG;
	return rotation;
}

void ComponentRigidBody::SetCenterOfMass(float3 center)
{
	physx::PxVec3 position(center.x, center.y, center.z);
	physx::PxTransform transform(position);
	rigidbody->setCMassLocalPose(transform);
}

float3 ComponentRigidBody::GetCenterOfMass() const
{
	physx::PxTransform transform = rigidbody->getCMassLocalPose();
	float3 center(transform.p.x, transform.p.y, transform.p.z);
	return center;
}

void ComponentRigidBody::SetInertiaTensor(float3 tensor)
{
	physx::PxVec3 phys_tensor(tensor.x, tensor.y, tensor.z);
	rigidbody->setMassSpaceInertiaTensor(phys_tensor);
}

float3 ComponentRigidBody::GetInertiaTensor() const
{
	physx::PxVec3 phys_tensor = rigidbody->getMassSpaceInertiaTensor();
	float3 tensor(phys_tensor.x, phys_tensor.y, phys_tensor.z);
	return tensor;
}

void ComponentRigidBody::SetSleepThreshold(float value)
{
	rigidbody->setSleepThreshold(value);
}

float ComponentRigidBody::GetSleepThreshold() const
{
	return rigidbody->getSleepThreshold();
}

void ComponentRigidBody::SetSolverIterations(uint count)
{
	rigidbody->setSolverIterationCounts(count);
}

uint ComponentRigidBody::GetSolverIterations() const
{
	uint iterations;
	uint velocity_iterations;
	rigidbody->getSolverIterationCounts(iterations, velocity_iterations);
	return iterations;
}

void ComponentRigidBody::SetVelocitySolverIterations(uint count)
{
	uint iterations;
	uint velocity_iterations;
	rigidbody->getSolverIterationCounts(iterations, velocity_iterations);
	rigidbody->setSolverIterationCounts(iterations, count);
}

uint ComponentRigidBody::GetVelocitySolverIterations() const
{
	uint iterations;
	uint velocity_iterations;
	rigidbody->getSolverIterationCounts(iterations, velocity_iterations);
	return velocity_iterations;
}

std::vector<physx::PxShape*> ComponentRigidBody::GetShapes() const
{
	std::vector<physx::PxShape*> ret;
	uint shapes_count = rigidbody->getNbShapes();
	for (uint i = 0; i < shapes_count; i++)
	{
		physx::PxShape* shape = nullptr;
		rigidbody->getShapes(&shape, 1, i);
		if (shape)
		{
			ret.push_back(shape);
		}
	}
	return ret;
}

void ComponentRigidBody::RemoveShape(physx::PxShape & shape) const
{
	if(rigidbody->getNbShapes() > 0)
	{
		rigidbody->detachShape(shape);
	}
}

physx::PxRigidDynamic * ComponentRigidBody::GetRigidBody() const
{
	return rigidbody;
}

void ComponentRigidBody::SetKinematicTarget(float3 destination)
{
	if (IsKinematic())
	{
		physx::PxVec3 position(destination.x, destination.y, destination.z);
		physx::PxTransform phys_transform(position);
		rigidbody->setKinematicTarget(phys_transform);
	}
}

physx::PxTransform ComponentRigidBody::GetTransform() const
{
	return rigidbody->getGlobalPose();
}

void ComponentRigidBody::SetTransform(float * transform)
{
	physx::PxMat44 mat(transform);
	physx::PxTransform phys_transform(mat);
	rigidbody->setGlobalPose(phys_transform);
	//float3 a = Quat(phys_transform.q.x, phys_transform.q.y, phys_transform.q.z, phys_transform.q.w).ToEulerXYZ() * RADTODEG;
	//CONSOLE_LOG("Set transform: %.3f,%.3f,%.3f", a.x, a.y, a.z);
}

void ComponentRigidBody::SetColliderScale(float3 scale)
{
	std::vector<physx::PxShape*> shapes = GetShapes();
	physx::PxVec3 phys_scale(scale.x, scale.y, scale.z);

	for (uint i = 0; i < shapes.size(); i++)
	{
		switch (shapes[i]->getGeometryType())
		{
		case physx::PxGeometryType::eBOX:

			break;
		case physx::PxGeometryType::eSPHERE:
			break;
		case physx::PxGeometryType::eCAPSULE:
			break;
		case physx::PxGeometryType::eTRIANGLEMESH:
		{
			physx::PxTriangleMeshGeometry mesh = shapes[i]->getGeometry().triangleMesh();
			physx::PxMaterial* mat = nullptr;
			shapes[i]->getMaterials(&mat, 1);
			mesh.scale = phys_scale;
			rigidbody->detachShape(*shapes[i]);
			App->physics->CreateShape(*rigidbody, mesh, *mat);
		}
			break;
		case physx::PxGeometryType::eHEIGHTFIELD:
			break;
		case physx::PxGeometryType::eCONVEXMESH:
		{
			physx::PxConvexMeshGeometry mesh = shapes[i]->getGeometry().convexMesh();
			physx::PxMaterial* mat = nullptr;
			shapes[i]->getMaterials(&mat, 1);
			mesh.scale = phys_scale;
			rigidbody->detachShape(*shapes[i]);
			App->physics->CreateShape(*rigidbody, mesh, *mat);
		}
			break;
		default:
			break;
		}
	}
}

void ComponentRigidBody::SetDynamicLocks(DynamicLocks lock_type, bool active)
{
	switch (lock_type)
	{
	case ComponentRigidBody::LinearX:
		rigidbody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, active);
		break;
	case ComponentRigidBody::LinearY:
		rigidbody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, active);
		break;
	case ComponentRigidBody::LinearZ:
		rigidbody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, active);
		break;
	case ComponentRigidBody::AngularX:
		rigidbody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, active);
		break;
	case ComponentRigidBody::AngularY:
		rigidbody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, active);
		break;
	case ComponentRigidBody::AngularZ:
		rigidbody->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, active);
		break;
	default:
		break;
	}
}

bool ComponentRigidBody::GetDynamicLocks(DynamicLocks lock_type) const
{
	bool ret = false;
	physx::PxRigidDynamicLockFlags flags = rigidbody->getRigidDynamicLockFlags();

	switch (lock_type)
	{
	case ComponentRigidBody::LinearX:
		ret = flags.isSet(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X);
		break;
	case ComponentRigidBody::LinearY:
		ret = flags.isSet(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y);
		break;
	case ComponentRigidBody::LinearZ:
		ret = flags.isSet(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z);
		break;
	case ComponentRigidBody::AngularX:
		ret = flags.isSet(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X);
		break;
	case ComponentRigidBody::AngularY:
		ret = flags.isSet(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y);
		break;
	case ComponentRigidBody::AngularZ:
		ret = flags.isSet(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
		break;
	default:
		break;
	}

	return ret;
}

void ComponentRigidBody::SetCCDMode(bool ccd)
{
	rigidbody->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, ccd);
}

bool ComponentRigidBody::IsCCDMode() const
{
	physx::PxRigidBodyFlags flag = rigidbody->getRigidBodyFlags();
	return flag.isSet(physx::PxRigidBodyFlag::eENABLE_CCD);
}

void ComponentRigidBody::SetNewRigidBody(physx::PxRigidDynamic * new_rigid)
{
	rigidbody->release();
	rigidbody = new_rigid;
	//SetInitValues();
}

void ComponentRigidBody::Save(Data & data) const
{
	data.AddInt("Type", GetType());
	data.AddBool("Active", IsActive());
	data.AddUInt("UUID", GetUID());
	data.AddFloat("Mass", GetMass());
	data.AddBool("IsUsingGravity", IsUsingGravity());
	data.AddBool("IsKinematic", IsKinematic());
	data.AddVector3("LinearVelocity", GetLinearVelocity());
	data.AddVector3("AngularVelocity", GetAngularVelocity());
	data.AddFloat("MaxAngularVelocity", GetMaxAngularVelocity());
	data.AddFloat("LinearDamping", GetLinearDamping());
	data.AddFloat("AngularDamping", GetAngularDamping());
	//data.AddVector3("Position", GetPosition());
	//data.AddVector3("Rotation", GetRotation());
	data.AddVector3("CenterMass", GetCenterOfMass());
	data.AddVector3("InertiaTensor", GetInertiaTensor());
	data.AddFloat("SleepThreshold", GetSleepThreshold());
	data.AddUInt("SolverIterations", GetSolverIterations());
	data.AddUInt("VelocitySolverIterations", GetVelocitySolverIterations());
	data.AddBool("LinearXLock", GetDynamicLocks(LinearX));
	data.AddBool("LinearYLock", GetDynamicLocks(LinearY));
	data.AddBool("LinearZLock", GetDynamicLocks(LinearZ));
	data.AddBool("AngularXLock", GetDynamicLocks(AngularX));
	data.AddBool("AngularYLock", GetDynamicLocks(AngularY));
	data.AddBool("AngularZLock", GetDynamicLocks(AngularZ));
	data.AddBool("IsCCDMode", IsCCDMode());
	/*std::vector<physx::PxShape*> shapes = GetShapes();
	data.AddInt("ShapesCount", shapes.size());
	for (std::vector<physx::PxShape*>::iterator it = shapes.begin(), int i = 0; it != shapes.end(); it++, i++)
	{
		data.CreateSection("Shape_" + std::to_string(i));
		physx::PxGeometryType::Enum type = (*it)->getGeometryType();
		data.AddInt("ShapeType", type);
		data.CloseSection();
	}*/
}

void ComponentRigidBody::Load(Data & data)
{
	SetType((ComponentType)data.GetInt("Type"));
	SetActive(data.GetBool("Active"));
	SetUID(data.GetUInt("UUID"));
	SetMass(data.GetFloat("Mass"));
	SetUseGravity(data.GetBool("IsUsingGravity"));
	SetKinematic(data.GetBool("IsKinematic"));
	SetLinearVelocity(data.GetVector3("LinearVelocity"));
	SetAngularVelocity(data.GetVector3("AngularVelocity"));
	SetMaxAngularVelocity(data.GetFloat("MaxAngularVelocity"));
	SetLinearDamping(data.GetFloat("LinearDamping"));
	SetAngularDamping(data.GetFloat("AngularDamping"));
	//SetPosition(data.GetVector3("Position"));
	//SetRotation(data.GetVector3("Rotation"));
	SetCenterOfMass(data.GetVector3("CenterMass"));
	SetInertiaTensor(data.GetVector3("InertiaTensor"));
	SetSleepThreshold(data.GetFloat("SleepThreshold"));
	SetSolverIterations(data.GetUInt("SolverIterations"));
	SetVelocitySolverIterations(data.GetUInt("VelocitySolverIterations"));
	SetDynamicLocks(LinearX, data.GetBool("LinearXLock"));
	SetDynamicLocks(LinearY, data.GetBool("LinearYLock"));
	SetDynamicLocks(LinearZ, data.GetBool("LinearZLock"));
	SetDynamicLocks(AngularX, data.GetBool("AngularXLock"));
	SetDynamicLocks(AngularY, data.GetBool("AngularYLock"));
	SetDynamicLocks(AngularZ, data.GetBool("AngularZLock"));
	SetCCDMode(data.GetBool("IsCCDMode"));
	/*int shapes_count = data.GetInt("ShapesCount");
	for (int i = 0; i < shapes_count; i++)
	{
		data.EnterSection("Shape_" + std::to_string(i));
		int type = data.GetInt("ShapeType");
		switch (type)
		{
		case physx::PxGeometryType::eSPHERE:
			GetGameObject()->AddComponent(Component::CompSphereCollider);
			break;
		case physx::PxGeometryType::ePLANE:
			break;
		case physx::PxGeometryType::eCAPSULE:
			GetGameObject()->AddComponent(Component::CompCapsuleCollider);
			break;
		case physx::PxGeometryType::eBOX:
			GetGameObject()->AddComponent(Component::CompBoxCollider);
			break;
		case physx::PxGeometryType::eCONVEXMESH:
			ComponentCollider* col = (ComponentCollider*)GetGameObject()->AddComponent(Component::CompMeshCollider);
			col->ChangeMeshToConvex(true);
			break;
		case physx::PxGeometryType::eTRIANGLEMESH:
			GetGameObject()->AddComponent(Component::CompMeshCollider);
			break;
		case physx::PxGeometryType::eHEIGHTFIELD:
			break;
		case physx::PxGeometryType::eGEOMETRY_COUNT:
			break;
		case physx::PxGeometryType::eINVALID:
			break;
		default:
			break;
		}
	}*/
}


