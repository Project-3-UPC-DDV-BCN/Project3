#pragma once
#include "Component.h"

namespace math
{
	class Quat;
	class float3;
}

class GameObject;
class Material;
class BlastModel;

class ComponentBlast :
	public Component
{
public:
	ComponentBlast(GameObject* attached_gameobject);
	~ComponentBlast();

	void SetTransform(float* transform);

	void Save(Data& data) const;
	void Load(Data& data);

public:
	Material* interior_material;
	BlastModel* blast_model;
};

