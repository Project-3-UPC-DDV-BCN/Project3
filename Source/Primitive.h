
#pragma once
#include "glmath.h"
#include "Color.h"
#include <vector>
#include "MathGeoLib/Frustum.h"
#include "MathGeoLib/AABB.h"
#include "Texture.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder,
	Primitive_CubeArray,
	Primitive_CubeIndices,
	Primitive_TexturedCube,
	Primitive_Ray,
	Primitive_Frustum,
};

class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	virtual void	CleanUp();
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;
	vec3 GetPosition() const;

public:
	
	Color color;
	mat4x4 transform;
	bool axis,wire;

protected:
	PrimitiveTypes type;
};

// ============================================
class pCube : public Primitive
{
public :
	pCube();
	pCube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
	float3 corners[8];
};

// ============================================
// ============================================
class pTexturedCube : public Primitive
{
public:
	pTexturedCube();
	pTexturedCube(float sizeX, float sizeY, float sizeZ);
	void SetTextures(Texture* textures_id[6]);
	void InnerRender() const;
	void SetPos(float3 new_pos);
public:
	vec3 size;
	float3 corners[8];
	Texture* textures_id[6];
	float3 pos;
};

// ============================================
// ============================================
typedef struct
{
	float CubeVertices[108];
}CubeV;
class pCubeVArray : public Primitive
{
public:
	pCubeVArray();
	pCubeVArray(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
private:
	CubeV vertices;
	uint vbo_id;
	float num_vertex;
};
// ============================================

// ============================================
typedef struct
{
	float CubeVertices[24];
	uint  Indices[36];
}CubeI;
class pCubeIndices : public Primitive
{
public:
	pCubeIndices();
	pCubeIndices(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	vec3 size;
private:
	CubeI vertices_indices;

	uint vbo_id;
	float num_vertex;
	uint ibo_id;
	float num_indices;
};
// ============================================
class pSphere : public Primitive
{
public:
	pSphere();
	pSphere(float radius);
	pSphere(float radius,uint rings,uint sectors);
	void InnerRender() const;
public:
	float radius;
	std::vector<float> vertices;
	std::vector<uint> indices;

	uint vbo_id;
	float num_vertex;
	uint ibo_id;
	float num_indices;
};

// ============================================
class pTexturedSphere : public Primitive
{
public:
	pTexturedSphere(float radius, Texture* texture);
	pTexturedSphere(float radius, uint rings, uint sectors, Texture* texture);
	void InnerRender() const;

public:
	void SetTexture(Texture* texture);

private:
	float radius;
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texcoords;
	std::vector<uint> indices;
	Texture* texture;

	uint vbo_id;
	float num_vertex;
	uint ibo_id;
	float num_indices;
	uint tbo_id;
	float num_tex_coords;
	uint nbo_id;
	float num_normals;
};

// ============================================
class pCylinder : public Primitive
{
public:
	pCylinder();
	pCylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class pLine : public Primitive
{
public:
	pLine();
	pLine(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
// ============================================
class pRay : public Primitive
{
public:
	pRay();
	pRay(float ox, float oy, float oz, float dx, float dy, float dz);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class pPlane : public Primitive
{
public:
	pPlane();
	pPlane(float x, float y, float z, float d);
	void InnerRender() const;
	void SetPos(float3 pos);
public:
	vec3 normal;
	float constant;
	float3 pos;
};
// ============================================
typedef struct
{
	float FrustumVertices[24];
	uint  Indices[36];
}FrustumI;
class pFrustum : public Primitive
{
public:
	pFrustum();
	pFrustum(float w1, float h1, float l, float w2, float h2);
	void InnerRender() const;
public:
	vec2 face1;
	int length;
	vec2 face2;
	FrustumI vertices_indices;

	uint vbo_id;
	float num_vertex;
	uint ibo_id;
	float num_indices;
};

class DebugFrustum : public Primitive
{
public:
	DebugFrustum(math::Frustum& frustum);
	void InnerRender() const;

private:
	float3 corners[8];
};

class DebugAABB : public Primitive
{
public:
	DebugAABB(math::AABB& aabb);
	void InnerRender() const;

private:
	float3 corners[8];
};