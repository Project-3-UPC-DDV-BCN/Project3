#include "ComponentJointFixed.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ComponentRigidBody.h"

ComponentJointFixed::ComponentJointFixed(GameObject* attached_gameobject)
{
	SetActive(true);
	SetType(ComponentType::CompFixedJoint);
	SetGameObject(attached_gameobject);
	SetName("Fixed_joint");

	rigidbody = (ComponentRigidBody*)attached_gameobject->GetComponent(Component::CompRigidBody);
	if (rigidbody != nullptr)
	{
		joint = App->physics->CreateFixedJoint(rigidbody->GetRigidBody(), rigidbody->GetTransform(), nullptr, physx::PxTransform(0,0,0));
		SetEnablePreprocessing(true);
		SetEnableCollisions(false);
	}
}

ComponentJointFixed::~ComponentJointFixed()
{
	joint->release();
}

void ComponentJointFixed::SetBreakForce(float force)
{
	float torque = GetBreakTorque();
	joint->setBreakForce(force, torque);
}

void ComponentJointFixed::SetBreakTorque(float torque)
{
	float force = GetBreakForce();
	joint->setBreakForce(force, torque);
}

float ComponentJointFixed::GetBreakForce() const
{
	float force;
	float torque;
	joint->getBreakForce(force, torque);
	return force;
}

float ComponentJointFixed::GetBreakTorque() const
{
	float force;
	float torque;
	joint->getBreakForce(force, torque);
	return torque;
}

bool ComponentJointFixed::IsBroken() const
{
	physx::PxConstraintFlags flag = joint->getConstraintFlags();
	return flag.isSet(physx::PxConstraintFlag::eBROKEN);
}

void ComponentJointFixed::SetConnectedBody(ComponentRigidBody * body)
{
	joint->setActors(rigidbody->GetRigidBody(), body->GetRigidBody());
}

ComponentRigidBody * ComponentJointFixed::GetConnectedBody()
{
	physx::PxRigidActor* connected = nullptr;
	physx::PxRigidActor* current = nullptr;
	joint->getActors(*&current, *&connected);
	GameObject* go = (GameObject*)connected->userData;
	ComponentRigidBody* body = (ComponentRigidBody*)go->GetComponent(Component::CompRigidBody);
	return body;
}

void ComponentJointFixed::SetMassScale(float mass)
{
	joint->setInvMassScale0(mass);
}

float ComponentJointFixed::GetMassScale() const
{
	return joint->getInvMassScale0();
}

void ComponentJointFixed::SetConnectedMassScale(float mass)
{
	joint->setInvMassScale1(mass);
}

float ComponentJointFixed::GetConnectedMassScale() const
{
	return joint->getInvMassScale1();
}

void ComponentJointFixed::SetEnableCollisions(bool enable)
{
	joint->setConstraintFlag(physx::PxConstraintFlag::eCOLLISION_ENABLED, enable);
}

bool ComponentJointFixed::GetEnableCollisions() const
{
	physx::PxConstraintFlags flags = joint->getConstraintFlags();
	return flags.isSet(physx::PxConstraintFlag::eCOLLISION_ENABLED);
}

void ComponentJointFixed::SetEnablePreprocessing(bool enable)
{
	joint->setConstraintFlag(physx::PxConstraintFlag::eDISABLE_PREPROCESSING, enable);
}

bool ComponentJointFixed::GetEnablePreprocessing() const
{
	physx::PxConstraintFlags flags = joint->getConstraintFlags();
	return flags.isSet(physx::PxConstraintFlag::eDISABLE_PREPROCESSING);
}
