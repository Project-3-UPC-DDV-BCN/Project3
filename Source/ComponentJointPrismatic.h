#pragma once
#include "Component.h"
#include "Nvidia/PhysX/Include/extensions/PxPrismaticJoint.h"

class ComponentRigidBody;

class ComponentJointPrismatic :
	public Component
{
public:
	ComponentJointPrismatic(GameObject* attached_gameobject);
	~ComponentJointPrismatic();

	void SetBreakForce(float force);
	void SetBreakTorque(float torque);
	float GetBreakForce() const;
	float GetBreakTorque() const;
	bool IsBroken() const;
	void SetConnectedBody(ComponentRigidBody* body);
	ComponentRigidBody* GetConnectedBody();
	void SetMassScale(float mass);
	float GetMassScale() const;
	void SetConnectedMassScale(float mass);
	float GetConnectedMassScale() const;
	void SetEnableCollisions(bool enable);
	bool GetEnableCollisions() const;
	void SetEnablePreprocessing(bool enable);
	bool GetEnablePreprocessing() const;

	void SetUpperLimit(float value);
	float GetUpperLimit() const;
	void SetLowerLimit(float value);
	float GetLowerLimit() const;
	void SetEnableLimits(bool enable);
	bool GetEnableLimits() const;

private:
	physx::PxPrismaticJoint* joint;
	ComponentRigidBody* rigidbody;
};

