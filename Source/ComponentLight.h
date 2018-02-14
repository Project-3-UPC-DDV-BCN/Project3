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

	bool CleanUp();

	void Save(Data& data) const;
	void Load(Data& data);

	void SetColor(Color color);
	Color GetColor() const;
	float* GetColorToEdit() const;
	float4 GetColorAsFloat4() const;

	LightType GetType() const;
	std::string GetTypeString() const;
	void SetTypeToDirectional();
	void SetTypeToPoint();
	void SetTypeToSpot();

	void SetDiffuse(float diffuse);
	void SetAmbient(float ambient);
	void SetSpecular(float specular);

	float GetDiffuse() const;
	float GetAmbient() const;
	float GetSpecular() const;
	float GetCutOff() const;
	float GetOuterCutOff() const;
	float* GetDiffuseToEdit() const;
	float* GetAmbientToEdit() const;
	float* GetSpecularToEdit() const;
	float* GetCutOffToEdit() const;
	float* GetOuterCutOffToEdit() const;

private:
	Color color = White;
	LightType type = SPOT_LIGHT;
	float diffuse = 10.0f;
	float ambient = 3.0f;
	float specular = 10.0f;
	float cutOff = 12.0f;
	float outercutOff = 12.0f;
};