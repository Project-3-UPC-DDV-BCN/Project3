#pragma once
#include "Component.h"
#include "Nvidia/PhysX/Include/extensions/PxFixedJoint.h"

class ComponentRigidBody;

class ComponentJointFixed :
	public Component
{
public:

	ComponentJointFixed(GameObject* attached_gameobject);
	~ComponentJointFixed();

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

private:
	physx::PxFixedJoint* joint;
	ComponentRigidBody* rigidbody;
};

