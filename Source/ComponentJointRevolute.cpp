#include "ComponentJointRevolute.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ComponentRigidBody.h"


ComponentJointRevolute::ComponentJointRevolute(GameObject* attached_gameobject)
{
	SetActive(true);
	SetType(ComponentType::CompDistanceJoint);
	SetGameObject(attached_gameobject);
	SetName("Distance_joint");

	rigidbody = (ComponentRigidBody*)attached_gameobject->GetComponent(Component::CompRigidBody);
	if (rigidbody != nullptr)
	{
		joint = App->physics->CreateRevoluteJoint(rigidbody->GetRigidBody(), rigidbody->GetTransform(), nullptr, physx::PxTransform(0, 0, 0));
		SetEnablePreprocessing(true);
		SetEnableCollisions(false);
	}
}

ComponentJointRevolute::~ComponentJointRevolute()
{
	if (joint->isReleasable())
	{
		joint->release();
	}
}

void ComponentJointRevolute::SetBreakForce(float force)
{
	float torque = GetBreakTorque();
	joint->setBreakForce(force, torque);
}

void ComponentJointRevolute::SetBreakTorque(float torque)
{
	float force = GetBreakForce();
	joint->setBreakForce(force, torque);
}

float ComponentJointRevolute::GetBreakForce() const
{
	float force;
	float torque;
	joint->getBreakForce(force, torque);
	return force;
}

float ComponentJointRevolute::GetBreakTorque() const
{
	float force;
	float torque;
	joint->getBreakForce(force, torque);
	return torque;
}

bool ComponentJointRevolute::IsBroken() const
{
	physx::PxConstraintFlags flag = joint->getConstraintFlags();
	return flag.isSet(physx::PxConstraintFlag::eBROKEN);
}

void ComponentJointRevolute::SetConnectedBody(ComponentRigidBody * body)
{
	joint->setActors(rigidbody->GetRigidBody(), body->GetRigidBody());
}

ComponentRigidBody * ComponentJointRevolute::GetConnectedBody()
{
	physx::PxRigidActor* connected = nullptr;
	physx::PxRigidActor* current = nullptr;
	joint->getActors(*&current, *&connected);
	GameObject* go = (GameObject*)connected->userData;
	ComponentRigidBody* body = (ComponentRigidBody*)go->GetComponent(Component::CompRigidBody);
	return body;
}

void ComponentJointRevolute::SetMassScale(float mass)
{
	joint->setInvMassScale0(mass);
}

float ComponentJointRevolute::GetMassScale() const
{
	return joint->getInvMassScale0();
}

void ComponentJointRevolute::SetConnectedMassScale(float mass)
{
	joint->setInvMassScale1(mass);
}

float ComponentJointRevolute::GetConnectedMassScale() const
{
	return joint->getInvMassScale1();
}

void ComponentJointRevolute::SetEnableCollisions(bool enable)
{
	joint->setConstraintFlag(physx::PxConstraintFlag::eCOLLISION_ENABLED, enable);
}

bool ComponentJointRevolute::GetEnableCollisions() const
{
	physx::PxConstraintFlags flags = joint->getConstraintFlags();
	return flags.isSet(physx::PxConstraintFlag::eCOLLISION_ENABLED);
}

void ComponentJointRevolute::SetEnablePreprocessing(bool enable)
{
	joint->setConstraintFlag(physx::PxConstraintFlag::eDISABLE_PREPROCESSING, enable);
}

bool ComponentJointRevolute::GetEnablePreprocessing() const
{
	physx::PxConstraintFlags flags = joint->getConstraintFlags();
	return flags.isSet(physx::PxConstraintFlag::eDISABLE_PREPROCESSING);
}

void ComponentJointRevolute::SetUpperLimit(float value)
{
	physx::PxJointAngularLimitPair limit = joint->getLimit();
	limit.upper = value;
	joint->setLimit(limit);
}

float ComponentJointRevolute::GetUpperLimit() const
{
	return joint->getLimit().upper;
}

void ComponentJointRevolute::SetLowerLimit(float value)
{
	physx::PxJointAngularLimitPair limit = joint->getLimit();
	limit.lower = value;
	joint->setLimit(limit);
}

float ComponentJointRevolute::GetLowerLimit() const
{
	return joint->getLimit().lower;
}

void ComponentJointRevolute::SetDriveVelocity(float value)
{
	joint->setDriveVelocity(value);
}

float ComponentJointRevolute::GetDriveVelocity() const
{
	return joint->getDriveVelocity();
}

void ComponentJointRevolute::SetDriveForceLimit(float value)
{
	joint->setDriveForceLimit(value);
}

float ComponentJointRevolute::GetDriveForceLimit() const
{
	return joint->getDriveForceLimit();
}

void ComponentJointRevolute::SetDriveGearRatio(float value)
{
	joint->setDriveGearRatio(value);
}

float ComponentJointRevolute::GetDriveGearRatio() const
{
	return joint->getDriveGearRatio();
}

void ComponentJointRevolute::SetEnableLimits(bool enable)
{
	joint->setRevoluteJointFlag(physx::PxRevoluteJointFlag::eLIMIT_ENABLED, enable);
}

bool ComponentJointRevolute::GetEnableLimits() const
{
	return joint->getRevoluteJointFlags().isSet(physx::PxRevoluteJointFlag::eLIMIT_ENABLED);
}