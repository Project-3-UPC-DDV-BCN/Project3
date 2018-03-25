#pragma once

#include <string>
#include "MathGeoLib/GeometryAll.h"
#include "Resource.h"
#include "Vertex.h"

class Data;

class Mesh : public Resource
{
public:
	Mesh();
	~Mesh();

	void Save(Data& data) const;
	bool Load(Data& data);
	void CreateMeta() const;
	void LoadToMemory();
	void UnloadFromMemory();

	void CreateVerticesFromData();
	void FillVerticesList();
private:
	//Must be call after setting the values to the mesh!!!
	void InitializeMesh();

public:
	uint id_indices; // id in VRAM
	uint num_indices;
	uint* indices;

	uint id_vertices_data; // id in VRAM
	uint num_vertices;
	
	float* vertices_data = nullptr;
	float* vertices = nullptr; //copy of the vertices for animation and aabb enclose

	//Don't use this! This still here waiting for a fix. Use box in MeshRenderer!
	AABB box;

	uint id_vao = 0;

	std::vector<Vertex> vertices_list;
};

