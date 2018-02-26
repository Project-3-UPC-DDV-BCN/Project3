#pragma once
#include "Component.h"
#include "Nvidia/PhysX/Include/extensions/PxSphericalJoint.h"

class ComponentRigidBody;

class ComponentJointSpherical :
	public Component
{
public:
	ComponentJointSpherical(GameObject* attached_gameobject);
	~ComponentJointSpherical();

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

	void SetLimitConeY(float y);
	float GetLimitConeY() const;
	void SetLimitConeZ(float z);
	float GetLimitConeZ() const;
	void SetEnableLimits(bool enable);
	bool GetEnableLimits() const;

private:
	physx::PxSphericalJoint* joint;
	ComponentRigidBody* rigidbody;
};

