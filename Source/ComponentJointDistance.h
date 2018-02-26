#pragma once
#include "Component.h"
#include "Nvidia/PhysX/Include/extensions/PxDistanceJoint.h"

class ComponentRigidBody;

class ComponentJointDistance :
	public Component
{
public:

	ComponentJointDistance(GameObject* attached_gameobject);
	~ComponentJointDistance();

	void SetBreakForce(float force);
	void SetBreakTorque(float torque);
	float GetBreakForce() const;
	float GetBreakTorque() const;
	bool IsBroken() const;
	void SetConnectedBody(GameObject* body);
	GameObject* GetConnectedBody();
	void SetMassScale(float mass);
	float GetMassScale() const;
	void SetConnectedMassScale(float mass);
	float GetConnectedMassScale() const;
	void SetEnableCollisions(bool enable);
	bool GetEnableCollisions() const;
	void SetEnablePreprocessing(bool enable);
	bool GetEnablePreprocessing() const;

	void SetMinDistance(float value);
	float GetMinDistance() const;
	void SetMaxDistance(float value);
	float GetMaxDistance() const;
	void SetTolerance(float value);
	float GetTolerance() const;
	void SetStiffness(float value);
	float GetStiffness() const;
	void SetDamping(float value);
	float GetDamping() const;
	void SetMaxDistanceEnabled(bool enabled);
	bool GetMaxDistanceEnabled() const;
	void SetMinDistanceEnabled(bool enabled);
	bool GetMinDistanceEnabled() const;
	void SetSpringEnabled(bool enabled);
	bool GetSpringEnabled() const;

	void Save(Data& data) const;
	void Load(Data& data);

private:
	physx::PxDistanceJoint* joint;
	ComponentRigidBody* rigidbody;
};

