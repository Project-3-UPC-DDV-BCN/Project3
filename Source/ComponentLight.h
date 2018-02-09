#pragma once
#include "Component.h"
#include "Color.h"

enum LightType {
	DIRECTIONAL_LIGHT = 0,
	POINT_LIGHT,
	SPOT_LIGHT,
	UNKNOWN_LIGHT
};

class ComponentLight :
	public Component
{
public:
	ComponentLight(GameObject* attached_gameobject);
	virtual ~ComponentLight();

	void Save(Data& data) const;
	void Load(Data& data);

	void SetColor(Color color);
	Color GetColor() const;
	float* GetColorToEdit() const;
	float4 GetColorAsFloat4() const;

	void SetIntensity(float intensity);
	float GetIntensity() const;

	LightType GetType() const;
	std::string GetTypeString() const;
	void SetTypeToDirectional();
	void SetTypeToPoint();
	void SetTypeToSpot();

private:
	Color color = White;
	float intensity = 1.0f;
	LightType type = POINT_LIGHT;
};