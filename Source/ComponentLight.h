#pragma once
#include "Component.h"
#include "Color.h"

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
	float4 GetColorAsFloat4() const;

	void SetIntensity(float intensity);
	float GetIntensity() const;

private:
	Color color = White;
	float intensity = 1.0f;
};

