#pragma once
#include "Component.h"
#include "Nvidia/PhysX/Include/extensions/PxRevoluteJoint.h"

class ComponentRigidBody;

class ComponentJointRevolute :
	public Component
{
public:
	ComponentJointRevolute(GameObject* attached_gameobject);
	~ComponentJointRevolute();

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
	void SetDriveVelocity(float value);
	float GetDriveVelocity() const;
	void SetDriveForceLimit(float value);
	float GetDriveForceLimit() const;
	void SetDriveGearRatio(float value);
	float GetDriveGearRatio() const;
	void SetEnableLimits(bool enable);
	bool GetEnableLimits() const;

private:
	physx::PxRevoluteJoint* joint;
	ComponentRigidBody* rigidbody;
};

