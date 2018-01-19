#pragma once
#include "Component.h"


class ComponentLight :
	public Component
{
public:
	ComponentLight(GameObject* attached_gameobject);
	virtual ~ComponentLight();

	void Save(Data& data) const;
	void Load(Data& data);

private:
};

