#pragma once
#include "Component.h"
#include "MathGeoLib/AABB.h"

class Mesh;
class Material;

class ComponentMeshRenderer :
	public Component
{
public:

	enum MeshType
	{
		NormalMesh, BlastMesh
	};

	ComponentMeshRenderer(GameObject* attached_gameobject);
	virtual ~ComponentMeshRenderer();

	Mesh* GetMesh() const;
	void SetMesh(Mesh* mesh);

	Material* GetMaterial() const;
	void SetMaterial(Material* material);

	void UpdateBoundingBox();

	void LoadToMemory();
	void UnloadFromMemory();

	void Save(Data& data) const;
	void Load(Data& data);

	void SetMeshType(MeshType type);
	MeshType GetMeshType() const;

	//USED IN BLAST MESH
	Material* GetInteriorMaterial() const;
	void SetInteriorMaterial(Material* material);

public:
	bool material_changed; 
	int material_indices_number;
	int material_indices_start;
	int interior_material_indices_number;
	int interior_material_indices_start;
	AABB bounding_box;
	////////////////////////////////////////
	bool has_light = true;
	float self_transparency = -1.0f;
private:
	Mesh* mesh;
	Material* material;
	MeshType mesh_type;

	//Blast Mesh
	Material * interior_material;
};

