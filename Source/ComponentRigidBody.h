#pragma once
#include "Component.h"

namespace physx
{
	class PxRigidDynamic;
	class PxShape;
	class PxTransform;
}

class ComponentRigidBody :
	public Component
{
public:

	enum DynamicLocks
	{
		LinearX, LinearY, LinearZ, AngularX, AngularY, AngularZ
	};

	ComponentRigidBody(GameObject* attached_gameobject);
	~ComponentRigidBody();

	void SetInitValues();

	void SetMass(float new_mass);
	float GetMass() const;
	void SetUseGravity(bool use);
	bool IsUsingGravity() const;
	void SetKinematic(bool kinematic);
	bool IsKinematic() const;
	void SetLinearVelocity(float3 linear_velocity);
	float3 GetLinearVelocity() const;
	void SetAngularVelocity(float3 angular_velocity);
	float3 GetAngularVelocity() const;
	void SetMaxAngularVelocity(float max_velocity);
	float GetMaxAngularVelocity() const;
	void SetLinearDamping(float linear_damping);
	float GetLinearDamping() const;
	void SetAngularDamping(float angular_damping);
	float GetAngularDamping() const;
	void AddForce(float3 force, int force_mode);
	void ClearForce(int force_mode);
	void AddTorque(float3 torque, int force_mode);
	void ClearTorque(int force_mode);
	bool IsSleeping() const;
	void SetToSleep();
	void WakeUp();
	void SetPosition(float3 new_position);
	float3 GetPosition() const;
	void SetRotation(float3 new_rotation);
	float3 GetRotation() const;
	void SetCenterOfMass(float3 center);
	float3 GetCenterOfMass() const;
	void SetInertiaTensor(float3 tensor);
	float3 GetInertiaTensor() const;
	void SetSleepThreshold(float value);
	float GetSleepThreshold() const;
	void SetSolverIterations(uint count);
	uint GetSolverIterations() const;
	void SetVelocitySolverIterations(uint count);
	uint GetVelocitySolverIterations() const;
	std::vector<physx::PxShape*> GetShapes() const;
	void RemoveShape(physx::PxShape& shape) const;
	physx::PxRigidDynamic* GetRigidBody() const;
	void SetKinematicTarget(float3 destination);
	physx::PxTransform GetTransform() const;
	void SetTransform(float* transform);
	void SetColliderScale(float3 scale);
	void SetDynamicLocks(DynamicLocks lock_type, bool active);
	bool GetDynamicLocks(DynamicLocks lock_type) const;
	void SetCCDMode(bool ccd);
	bool IsCCDMode() const;

	void EnableShapes();
	void DisableShapes();

	void SetNewRigidBody(physx::PxRigidDynamic* new_rigid);

	void Save(Data& data) const;
	void Load(Data& data);

	void DrawColliders();
	
private:
	bool use_gravity;
	bool is_kinematic;

	physx::PxRigidDynamic* rigidbody;

	float3 curr_rot;
	//Quat curr_rot_quat;
};

