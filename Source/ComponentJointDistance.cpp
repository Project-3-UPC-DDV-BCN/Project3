#include "ComponentJointDistance.h"
#include "GameObject.h"
#include "ModulePhysics.h"
#include "Application.h"
#include "ComponentRigidBody.h"


ComponentJointDistance::ComponentJointDistance(GameObject* attached_gameobject)
{
	SetActive(true);
	SetType(ComponentType::CompDistanceJoint);
	SetGameObject(attached_gameobject);
	SetName("Distance_joint");

	rigidbody = (ComponentRigidBody*)attached_gameobject->GetComponent(Component::CompRigidBody);
	if (rigidbody != nullptr)
	{
		physx::PxVec3 offset = physx::PxVec3(0, 0, 0);
		joint = App->physics->CreateDistanceJoint(rigidbody->GetRigidBody(), physx::PxTransform(offset), nullptr, physx::PxTransform(offset));
		joint->setConstraintFlag(physx::PxConstraintFlag::eVISUALIZATION, true);
		/*joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
		joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, true);
		SetMaxDistance(0.1f);
		SetMinDistance(0);
		SetTolerance(0.1f);*/
		SetEnablePreprocessing(true);
		SetEnableCollisions(false);
	}
}

ComponentJointDistance::~ComponentJointDistance()
{
	joint->release();
}

void ComponentJointDistance::SetBreakForce(float force)
{
	float torque = GetBreakTorque();
	joint->setBreakForce(force, torque);
}

void ComponentJointDistance::SetBreakTorque(float torque)
{
	float force = GetBreakForce();
	joint->setBreakForce(force, torque);
}

float ComponentJointDistance::GetBreakForce() const
{
	float force;
	float torque;
	joint->getBreakForce(force, torque);
	return force;
}

float ComponentJointDistance::GetBreakTorque() const
{
	float force;
	float torque;
	joint->getBreakForce(force, torque);
	return torque;
}

bool ComponentJointDistance::IsBroken() const
{
	physx::PxConstraintFlags flag = joint->getConstraintFlags();
	return flag.isSet(physx::PxConstraintFlag::eBROKEN);
}

void ComponentJointDistance::SetConnectedBody(GameObject * body)
{
	CONSOLE_LOG("1");
	physx::PxRigidActor* actor = nullptr;
	if (body)
	{
		CONSOLE_LOG("2");
		ComponentRigidBody* rb = (ComponentRigidBody*)body->GetComponent(Component::CompRigidBody);
		if (rb)
		{
			CONSOLE_LOG("3");
			actor = rb->GetRigidBody();
		}
	}
	physx::PxVec3 offset = physx::PxVec3(0, 0, 0);
	CONSOLE_LOG("4");
	joint->setActors(rigidbody->GetRigidBody(), actor);
	CONSOLE_LOG("5");
	joint->setLocalPose(physx::PxJointActorIndex::eACTOR1, physx::PxTransform(offset));
	CONSOLE_LOG("6");
}

GameObject * ComponentJointDistance::GetConnectedBody()
{
	physx::PxRigidActor* connected = nullptr;
	physx::PxRigidActor* current = nullptr;
	joint->getActors(*&current, *&connected);
	GameObject* go = nullptr;
	if (connected)
	{
		go = (GameObject*)connected->userData;
	}
	
	return go;
}

void ComponentJointDistance::SetMassScale(float mass)
{
	joint->setInvMassScale0(mass);
}

float ComponentJointDistance::GetMassScale() const
{
	return joint->getInvMassScale0();
}

void ComponentJointDistance::SetConnectedMassScale(float mass)
{
	joint->setInvMassScale1(mass);
}

float ComponentJointDistance::GetConnectedMassScale() const
{
	return joint->getInvMassScale1();
}

void ComponentJointDistance::SetEnableCollisions(bool enable)
{
	joint->setConstraintFlag(physx::PxConstraintFlag::eCOLLISION_ENABLED, enable);
}

bool ComponentJointDistance::GetEnableCollisions() const
{
	physx::PxConstraintFlags flags = joint->getConstraintFlags(); 
	return flags.isSet(physx::PxConstraintFlag::eCOLLISION_ENABLED);
}

void ComponentJointDistance::SetEnablePreprocessing(bool enable)
{
	joint->setConstraintFlag(physx::PxConstraintFlag::eDISABLE_PREPROCESSING, enable);
}

bool ComponentJointDistance::GetEnablePreprocessing() const
{
	physx::PxConstraintFlags flags = joint->getConstraintFlags();
	return flags.isSet(physx::PxConstraintFlag::eDISABLE_PREPROCESSING);
}

void ComponentJointDistance::SetMinDistance(float value)
{
	joint->setMinDistance(value);
}

float ComponentJointDistance::GetMinDistance() const
{
	return joint->getMinDistance();
}

void ComponentJointDistance::SetMaxDistance(float value)
{
	joint->setMaxDistance(value);
}

float ComponentJointDistance::GetMaxDistance() const
{
	return joint->getMaxDistance();
}

void ComponentJointDistance::SetTolerance(float value)
{
	joint->setTolerance(value);
}

float ComponentJointDistance::GetTolerance() const
{
	return joint->getTolerance();
}

void ComponentJointDistance::SetStiffness(float value)
{
	joint->setStiffness(value);
}

float ComponentJointDistance::GetStiffness() const
{
	return joint->getStiffness();
}

void ComponentJointDistance::SetDamping(float value)
{
	joint->setDamping(value);
}

float ComponentJointDistance::GetDamping() const
{
	return joint->getDamping();
}

void ComponentJointDistance::SetMaxDistanceEnabled(bool enabled)
{
	joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, enabled);
}

bool ComponentJointDistance::GetMaxDistanceEnabled() const
{
	return joint->getDistanceJointFlags().isSet(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED);
}

void ComponentJointDistance::SetMinDistanceEnabled(bool enabled)
{
	joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, enabled);
}

bool ComponentJointDistance::GetMinDistanceEnabled() const
{
	return joint->getDistanceJointFlags().isSet(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED);
}

void ComponentJointDistance::SetSpringEnabled(bool enabled)
{
	joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eSPRING_ENABLED, enabled);
}

bool ComponentJointDistance::GetSpringEnabled() const
{
	return joint->getDistanceJointFlags().isSet(physx::PxDistanceJointFlag::eSPRING_ENABLED);
}

void ComponentJointDistance::Save(Data & data) const
{
}

void ComponentJointDistance::Load(Data & data)
{
}
