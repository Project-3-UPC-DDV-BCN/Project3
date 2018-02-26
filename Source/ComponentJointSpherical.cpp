#include "ComponentJointSpherical.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ComponentRigidBody.h"


ComponentJointSpherical::ComponentJointSpherical(GameObject* attached_gameobject)
{
	SetActive(true);
	SetType(ComponentType::CompDistanceJoint);
	SetGameObject(attached_gameobject);
	SetName("Distance_joint");

	rigidbody = (ComponentRigidBody*)attached_gameobject->GetComponent(Component::CompRigidBody);
	if (rigidbody != nullptr)
	{
		joint = App->physics->CreateSphericalJoint(rigidbody->GetRigidBody(), rigidbody->GetTransform(), nullptr, physx::PxTransform(0, 0, 0));
		SetEnablePreprocessing(true);
		SetEnableCollisions(false);
	}
}

ComponentJointSpherical::~ComponentJointSpherical()
{
	joint->release();
}

void ComponentJointSpherical::SetBreakForce(float force)
{
	float torque = GetBreakTorque();
	joint->setBreakForce(force, torque);
}

void ComponentJointSpherical::SetBreakTorque(float torque)
{
	float force = GetBreakForce();
	joint->setBreakForce(force, torque);
}

float ComponentJointSpherical::GetBreakForce() const
{
	float force;
	float torque;
	joint->getBreakForce(force, torque);
	return force;
}

float ComponentJointSpherical::GetBreakTorque() const
{
	float force;
	float torque;
	joint->getBreakForce(force, torque);
	return torque;
}

bool ComponentJointSpherical::IsBroken() const
{
	physx::PxConstraintFlags flag = joint->getConstraintFlags();
	return flag.isSet(physx::PxConstraintFlag::eBROKEN);
}

void ComponentJointSpherical::SetConnectedBody(ComponentRigidBody * body)
{
	joint->setActors(rigidbody->GetRigidBody(), body->GetRigidBody());
}

ComponentRigidBody * ComponentJointSpherical::GetConnectedBody()
{
	physx::PxRigidActor* connected = nullptr;
	physx::PxRigidActor* current = nullptr;
	joint->getActors(*&current, *&connected);
	GameObject* go = (GameObject*)connected->userData;
	ComponentRigidBody* body = (ComponentRigidBody*)go->GetComponent(Component::CompRigidBody);
	return body;
}

void ComponentJointSpherical::SetMassScale(float mass)
{
	joint->setInvMassScale0(mass);
}

float ComponentJointSpherical::GetMassScale() const
{
	return joint->getInvMassScale0();
}

void ComponentJointSpherical::SetConnectedMassScale(float mass)
{
	joint->setInvMassScale1(mass);
}

float ComponentJointSpherical::GetConnectedMassScale() const
{
	return joint->getInvMassScale1();
}

void ComponentJointSpherical::SetEnableCollisions(bool enable)
{
	joint->setConstraintFlag(physx::PxConstraintFlag::eCOLLISION_ENABLED, enable);
}

bool ComponentJointSpherical::GetEnableCollisions() const
{
	physx::PxConstraintFlags flags = joint->getConstraintFlags();
	return flags.isSet(physx::PxConstraintFlag::eCOLLISION_ENABLED);
}

void ComponentJointSpherical::SetEnablePreprocessing(bool enable)
{
	joint->setConstraintFlag(physx::PxConstraintFlag::eDISABLE_PREPROCESSING, enable);
}

bool ComponentJointSpherical::GetEnablePreprocessing() const
{
	physx::PxConstraintFlags flags = joint->getConstraintFlags();
	return flags.isSet(physx::PxConstraintFlag::eDISABLE_PREPROCESSING);
}

void ComponentJointSpherical::SetLimitConeY(float y)
{
	physx::PxJointLimitCone cone = joint->getLimitCone();
	cone.yAngle = y;
	joint->setLimitCone(cone);
}

float ComponentJointSpherical::GetLimitConeY() const
{
	return joint->getLimitCone().yAngle;
}

void ComponentJointSpherical::SetLimitConeZ(float z)
{
	physx::PxJointLimitCone cone = joint->getLimitCone();
	cone.zAngle = z;
	joint->setLimitCone(cone);
}

float ComponentJointSpherical::GetLimitConeZ() const
{
	return joint->getLimitCone().zAngle;
}

void ComponentJointSpherical::SetEnableLimits(bool enable)
{
	joint->setSphericalJointFlag(physx::PxSphericalJointFlag::eLIMIT_ENABLED, enable);
}

bool ComponentJointSpherical::GetEnableLimits() const
{
	return joint->getSphericalJointFlags().isSet(physx::PxSphericalJointFlag::eLIMIT_ENABLED);
}