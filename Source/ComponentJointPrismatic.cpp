#include "ComponentJointPrismatic.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ComponentRigidBody.h"


ComponentJointPrismatic::ComponentJointPrismatic(GameObject* attached_gameobject)
{
	SetActive(true);
	SetType(ComponentType::CompDistanceJoint);
	SetGameObject(attached_gameobject);
	SetName("Prismatic_joint");

	rigidbody = (ComponentRigidBody*)attached_gameobject->GetComponent(Component::CompRigidBody);
	if (rigidbody != nullptr)
	{
		joint = App->physics->CreatePrismaticJoint(rigidbody->GetRigidBody(), rigidbody->GetTransform(), nullptr, physx::PxTransform(0, 0, 0));
		SetEnablePreprocessing(true);
		SetEnableCollisions(false);
	}
}

ComponentJointPrismatic::~ComponentJointPrismatic()
{
	if (joint->isReleasable())
	{
		joint->release();
	}
}

void ComponentJointPrismatic::SetBreakForce(float force)
{
	float torque = GetBreakTorque();
	joint->setBreakForce(force, torque);
}

void ComponentJointPrismatic::SetBreakTorque(float torque)
{
	float force = GetBreakForce();
	joint->setBreakForce(force, torque);
}

float ComponentJointPrismatic::GetBreakForce() const
{
	float force;
	float torque;
	joint->getBreakForce(force, torque);
	return force;
}

float ComponentJointPrismatic::GetBreakTorque() const
{
	float force;
	float torque;
	joint->getBreakForce(force, torque);
	return torque;
}

bool ComponentJointPrismatic::IsBroken() const
{
	physx::PxConstraintFlags flag = joint->getConstraintFlags();
	return flag.isSet(physx::PxConstraintFlag::eBROKEN);
}

void ComponentJointPrismatic::SetConnectedBody(ComponentRigidBody * body)
{
	joint->setActors(rigidbody->GetRigidBody(), body->GetRigidBody());
}

ComponentRigidBody * ComponentJointPrismatic::GetConnectedBody()
{
	physx::PxRigidActor* connected = nullptr;
	physx::PxRigidActor* current = nullptr;
	joint->getActors(*&current, *&connected);
	GameObject* go = (GameObject*)connected->userData;
	ComponentRigidBody* body = (ComponentRigidBody*)go->GetComponent(Component::CompRigidBody);
	return body;
}

void ComponentJointPrismatic::SetMassScale(float mass)
{
	joint->setInvMassScale0(mass);
}

float ComponentJointPrismatic::GetMassScale() const
{
	return joint->getInvMassScale0();
}

void ComponentJointPrismatic::SetConnectedMassScale(float mass)
{
	joint->setInvMassScale1(mass);
}

float ComponentJointPrismatic::GetConnectedMassScale() const
{
	return joint->getInvMassScale1();
}

void ComponentJointPrismatic::SetEnableCollisions(bool enable)
{
	joint->setConstraintFlag(physx::PxConstraintFlag::eCOLLISION_ENABLED, enable);
}

bool ComponentJointPrismatic::GetEnableCollisions() const
{
	physx::PxConstraintFlags flags = joint->getConstraintFlags();
	return flags.isSet(physx::PxConstraintFlag::eCOLLISION_ENABLED);
}

void ComponentJointPrismatic::SetEnablePreprocessing(bool enable)
{
	joint->setConstraintFlag(physx::PxConstraintFlag::eDISABLE_PREPROCESSING, enable);
}

bool ComponentJointPrismatic::GetEnablePreprocessing() const
{
	physx::PxConstraintFlags flags = joint->getConstraintFlags();
	return flags.isSet(physx::PxConstraintFlag::eDISABLE_PREPROCESSING);
}

void ComponentJointPrismatic::SetUpperLimit(float value)
{
	physx::PxJointLinearLimitPair limit = joint->getLimit();
	limit.upper = value;
	joint->setLimit(limit);
}

float ComponentJointPrismatic::GetUpperLimit() const
{
	return joint->getLimit().upper;
}

void ComponentJointPrismatic::SetLowerLimit(float value)
{
	physx::PxJointLinearLimitPair limit = joint->getLimit();
	limit.lower = value;
	joint->setLimit(limit);
}

float ComponentJointPrismatic::GetLowerLimit() const
{
	return joint->getLimit().lower;
}

void ComponentJointPrismatic::SetEnableLimits(bool enable)
{
	joint->setPrismaticJointFlag(physx::PxPrismaticJointFlag::eLIMIT_ENABLED, enable);
}

bool ComponentJointPrismatic::GetEnableLimits() const
{
	return joint->getPrismaticJointFlags().isSet(physx::PxPrismaticJointFlag::eLIMIT_ENABLED);
}
