#pragma once
#include "Component.h"

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

	//USED FOR BLAST MESHSES//

	Material* GetInteriorMaterial() const;
	void SetInteriorMaterial(Material* material);

	int material_indices_number;
	int material_indices_start;
	int interior_material_indices_number;
	int interior_material_indices_start;

	///////////////////////////////////

	void SetMeshType(MeshType type);
	MeshType GetMeshType() const;

private:
	Mesh* mesh;
	Material* material;
	Material* interior_material; //Only for Blast Mesh
	MeshType mesh_type;
};

