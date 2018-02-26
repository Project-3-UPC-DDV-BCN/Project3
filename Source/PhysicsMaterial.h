#pragma once
#include "Resource.h"

class PhysicsMaterial :
	public Resource
{
public:
	PhysicsMaterial();
	~PhysicsMaterial();

	void SetStaticFriction(float friction);
	float GetStaticFriction() const;
	void SetDynamicFriction(float friction);
	float GetDynamicFriction() const;
	void SetRestitution(float restitution);
	float GetRestitution() const;
	void SetFrictionMode(uint mode);
	uint GetFrictionMode() const;
	void SetRestitutionMode(uint mode);
	uint GetRestitutionMode() const;

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

private:
	float static_friction;
	float dynamic_friction;
	float restitution;
	uint friction_combine_mode;
	uint restitution_combine_mode;
};

