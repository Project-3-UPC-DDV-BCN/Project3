#pragma once
#include "Component.h"
#include "MathGeoLib\MathGeoLib.h"

class ComponentTransform :
	public Component
{
public:
	ComponentTransform(GameObject* attached_gameobject, bool is_particle = false);
	virtual ~ComponentTransform();

	void SetPosition(float3 position);
	float3 GetGlobalPosition() const;
	float3 GetLocalPosition() const;
	void SetRotation(float3 rotation);
	float3 GetGlobalRotation() const;
	float3 GetLocalRotation() const;
	void SetScale(float3 scale);
	float3 GetGlobalScale() const;
	float3 GetLocalScale() const;
	void UpdateGlobalMatrix();
	const float4x4 GetMatrix() const;
	const float* GetOpenGLMatrix() const;
	void SetMatrix(const float4x4 &matrix);

	void Save(Data& data) const;
	void Load(Data& data);

private:
	float3 position;
	Quat rotation;
	float3 shown_rotation;
	float3 scale;

	float3 global_pos;
	float3 global_rot;
	float3 global_scale;

	float4x4 transform_matrix;

	bool is_particle; 
};

