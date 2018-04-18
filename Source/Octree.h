#pragma once

#include <list>
#include "MathGeoLib/AABB.h"

#define MAX_DIVISIONS 3

class ComponentMeshRenderer;

class OctreeNode
{
public:
	OctreeNode(AABB& box);
	~OctreeNode();

	bool NodeIsFull() const;
	void InsertInNode(ComponentMeshRenderer* node_content);
	void EraseInNode(ComponentMeshRenderer* mesh);
	void DivideNode();
	void ClearNode();
	void CollectIntersections(std::vector<ComponentMeshRenderer*>& intersections_list, AABB* bounding_box);
	void DebugDrawNode();

public:
	AABB node_box;
	std::list<ComponentMeshRenderer*> node_contents;
	OctreeNode* node_childs[8];
	int division_level = 0;
};

class Octree
{
public:
	Octree();
	~Octree();
	void Create(float3 min_point, float3 max_point);
	void Clear();
	void Update();
	void Insert(ComponentMeshRenderer* mesh);
	void Erase(ComponentMeshRenderer* mesh);
	void CollectIntersections(std::vector<ComponentMeshRenderer*>& intersections_list, AABB* bounding_box);
	void DebugDraw();
	void CalculateNewSize(float3 box_min_point, float3 box_max_point);

private:
	OctreeNode* root_node = nullptr;

public:
	bool update_tree;
	float3 min_point;
	float3 max_point;
};

