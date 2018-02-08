#ifndef _H_DEBUG_DRAW__
#define _H_DEBUG_DRAW__

#include "Component.h"
#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Math\float4x4.h"
#include "Glew\include\glew.h"

class ComponentCamera;

class DebugShape
{
public:
	DebugShape(uint num_vertices, float* vertices, uint num_indices, uint* indices);

	void CleanUp();

	void SetTransform(float4x4 transform);
	void SetColour(float4 colour);
	void SetMode(int mode);

	uint GetNumVertices();
	float* GetVertices();
	uint GetNumIndices();
	uint* GetIndices();
	float4 GetColour();
	float4x4 GetTransform();
	int GetMode();

private:
	uint     num_vertices = 0;
	float*   vertices = nullptr;
	uint     num_indices = 0;
	uint*    indices = nullptr;

	float4   colour = float4::zero;

	float4x4 transform = float4x4::identity;
	int      mode = 0x0004;
};

class DebugDraw
{
public:
	DebugDraw();

	void Line(float3 start, float3 end);
	void Quad(float3 center, float2 size);
	void Quad(float4x4 transform, float2 size, float4 colour = float4(1.0f, 1.0f, 1.0f, 1.0f));
	void Circle(float4x4 transform, float rad, float4 colour = float4(1.0f, 1.0f, 1.0f, 1.0f));

	void Render(ComponentCamera* cameras);
	void Clear();

private:
	void AddShape(DebugShape shape);

private:
	std::vector<DebugShape> shapes;
};

#endif // !_H_DEBUG_DRAW__