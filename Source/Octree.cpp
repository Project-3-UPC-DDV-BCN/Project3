#include "Octree.h"
#include "Globals.h"
#include "Primitive.h"

OctreeNode::OctreeNode(AABB& box)
{
	node_childs[0] = node_childs[1] = node_childs[2] = node_childs[3] =
	node_childs[4] = node_childs[5] = node_childs[6] = node_childs[7] = nullptr;
	node_box = box;
}

OctreeNode::~OctreeNode()
{
	ClearNode();
}

bool OctreeNode::NodeIsFull() const
{
	return node_contents.size() == 2;
}

void OctreeNode::InsertInNode(AABB * node_content)
{
	if (node_childs[0] == nullptr/* && root_parent_node->division_level < MAX_DIVISIONS*/)
	{
		if (!NodeIsFull())
		{
			node_contents.push_back(node_content);
		}
		else
		{
			DivideNode();
		}
	}

	if (node_childs[0] != nullptr)
	{
		for (int i = 0; i < 8; i++)
		{
			if (node_childs[i]->node_box.Intersects(*node_content))
			{
				node_childs[i]->InsertInNode(node_content);
				break;
			}
		}
	}
}

void OctreeNode::EraseInNode(AABB * bounding_box)
{
	if (std::find(node_contents.begin(), node_contents.end(), bounding_box) != node_contents.end())
	{
		node_contents.remove(bounding_box);
	}
	
	if (node_childs[0] != nullptr)
	{
		bool delete_childs = false;
		for (int i = 0; i < 8; i++)
		{
			node_childs[i]->EraseInNode(bounding_box);
			delete_childs = node_childs[i]->node_contents.empty();
			if (!delete_childs) break;
		}
		if (delete_childs)
		{
			ClearNode();
		}
	}
}

void OctreeNode::DivideNode()
{
	AABB child_box;
	float3 childs_side_length = node_box.Size() / 2;
	float3 center = node_box.CenterPoint();

	int child_index = 0;
	for (int x = 0; x < 2; x++)
	{
		for (int y = 0; y < 2; y++)
		{
			for (int z = 0; z < 2; z++)
			{
				float3 min_child_point(
					node_box.minPoint.x + z * childs_side_length.x,
					node_box.minPoint.y + y * childs_side_length.y, 
					node_box.minPoint.z + x * childs_side_length.z
				);

				float3 max_child_point(
					min_child_point.x + childs_side_length.x,
					min_child_point.y + childs_side_length.y,
					min_child_point.z + childs_side_length.z
				);
				child_box.minPoint = min_child_point;
				child_box.maxPoint = max_child_point;
				node_childs[child_index] = new OctreeNode(child_box);
				child_index++;
			}
		}
	}
}

void OctreeNode::ClearNode()
{
	for (int i = 0; i < 8; ++i)
	{
		RELEASE(node_childs[i]);
	}
}

void OctreeNode::CollectIntersections(std::list<AABB*>& intersections_list, AABB * bounding_box)
{
	if (node_childs[0] != nullptr)
	{
		for (int i = 0; i < 8; i++)
		{
			if (node_childs[i]->node_box.Intersects(*bounding_box))
			{
				node_childs[i]->CollectIntersections(intersections_list, bounding_box);
			}
		}
	}

	for (std::list<AABB*>::iterator it = node_contents.begin(); it != node_contents.end(); it++)
	{
		if ((*it)->Intersects(*bounding_box))
		{
			intersections_list.push_back(*it);
		}
	}
}

//void OctreeNode::ResizeNode()
//{
//}

void OctreeNode::DebugDrawNode()
{
	if (node_childs[0] != nullptr)
	{
		for (int i = 0; i < 8; i++)
		{
			node_childs[i]->DebugDrawNode();
		}
	}
	DebugAABB aabb(node_box);
	aabb.color = { 0,1,1,1 };
	aabb.Render();
}

//////////////////////////////////////////////

Octree::Octree()
{
	update_tree = false;
}

Octree::~Octree()
{
	Clear();
}

void Octree::Create(float3 min_point, float3 max_point)
{
	Clear();
	AABB box(min_point, max_point);
	root_node = new OctreeNode(box);
	this->min_point = min_point;
	this->max_point = max_point;
	update_tree = true;
}

void Octree::Clear()
{
	RELEASE(root_node);
}

void Octree::Update()
{
	Create(min_point, max_point);
}

void Octree::Insert(AABB * bounding_box)
{
	update_tree = false;
	if (root_node != nullptr)
	{
		//If the box that we need to insert is out of the octree, we will need to update the octree.
		if (bounding_box->minPoint.x < min_point.x)
		{
			min_point.x = bounding_box->minPoint.x;
			update_tree = true;
		}
		if (bounding_box->minPoint.y < min_point.y) 
		{
			min_point.y = bounding_box->minPoint.y;
			update_tree = true;
		}
		if (bounding_box->minPoint.z < min_point.z) 
		{ 
			min_point.z = bounding_box->minPoint.z;
			update_tree = true;
		}
		if (bounding_box->maxPoint.x > max_point.x)
		{
			max_point.x = bounding_box->maxPoint.x;
			update_tree = true;
		}
		if (bounding_box->maxPoint.y > max_point.y)
		{
			max_point.y = bounding_box->maxPoint.y;
			update_tree = true;
		}
		if (bounding_box->maxPoint.z > max_point.z)
		{
			max_point.z = bounding_box->maxPoint.z;
			update_tree = true;
		}

		if (!update_tree)
		{
			root_node->InsertInNode(bounding_box);
		}
		
	}	
}

void Octree::Erase(AABB * bounding_box)
{
	if (root_node != nullptr)
	{
		//If the box that we need to delete is at any corner of the octree, we will need to update the octree.
		if (bounding_box->minPoint.x == min_point.x || bounding_box->minPoint.y == min_point.y || bounding_box->minPoint.z == min_point.z ||
			bounding_box->maxPoint.x == max_point.x || bounding_box->maxPoint.y == max_point.y || bounding_box->maxPoint.z == max_point.z)
		{
			update_tree = true;
		}

		//If we need to update the octree, we will handle that in the scene update if not, just erase the box
		if (!update_tree)
		{
			root_node->EraseInNode(bounding_box);
		}
	}
}

void Octree::CollectIntersections(std::list<AABB*>& intersections_list, AABB* bounding_box)
{
	if (root_node != nullptr)
	{
		if (bounding_box->Intersects(root_node->node_box))
		{
			root_node->CollectIntersections(intersections_list, bounding_box);
		}
	}
}

void Octree::DebugDraw()
{
	if (root_node != nullptr)
	{
		root_node->DebugDrawNode();
	}
}

void Octree::CalculateNewSize(float3 box_min_point, float3 box_max_point)
{
	//If the box that we need to insert is out of the octree, we will need to update the octree.
	if (box_min_point.x < min_point.x)
	{
		min_point.x = box_min_point.x;
	}
	if (box_min_point.y < min_point.y)
	{
		min_point.y = box_min_point.y;
	}
	if (box_min_point.z < min_point.z)
	{
		min_point.z = box_min_point.z;
	}
	if (box_max_point.x > max_point.x)
	{
		max_point.x = box_max_point.x;
	}
	if (box_max_point.y > max_point.y)
	{
		max_point.y = box_max_point.y;
	}
	if (box_max_point.z > max_point.z)
	{
		max_point.z = box_max_point.z;
	}
}