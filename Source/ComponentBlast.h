#pragma once
#include "Component.h"

class GameObject;
class Material;

class ComponentBlast :
	public Component
{
public:
	ComponentBlast(GameObject* attached_gameobject);
	~ComponentBlast();

	void Save(Data& data) const;
	void Load(Data& data);

public:
	Material* interior_material;
};

