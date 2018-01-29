#pragma once

#include "Component.h"

class BlastMesh;
class Material;
class Data;
class GameObject;

class ComponentBlastMeshRenderer :
	public Component
{
public:
	ComponentBlastMeshRenderer(GameObject* attached_gameobject);
	~ComponentBlastMeshRenderer();

	BlastMesh* GetMesh() const;
	void SetMesh(BlastMesh* mesh);

	Material* GetMaterial() const;
	void SetMaterial(Material* texture);
	Material* GetInteriorMaterial() const;
	void SetInteriorMaterial(Material* texture);

	void UpdateBoundingBox();
	 
	void LoadToMemory();
	void UnloadFromMemory();

	void Save(Data& data) const;
	void Load(Data& data);

private:
	BlastMesh * mesh;
	Material* material;
	Material* interior_material;
};

