#pragma once

#include "Resource.h"
#include "MathGeoLib/Geometry/AABB.h"

class Material;

class BlastMesh :
	public Resource
{
public:
	BlastMesh();
	~BlastMesh();

	uint id_indices; // id in VRAM
	uint num_indices;
	uint* indices;

	uint id_vertices; // id in VRAM
	uint num_vertices;
	float* vertices;

	uint id_normals;
	float* normals;

	uint id_colors;
	float* colors;

	uint id_texture_coords;
	float* texture_coords;

	AABB box;

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

	Material* interior_material;
};

