#include "Vertex.h"
#include "Globals.h"

Vertex::Vertex()
{
}

Vertex::~Vertex()
{
}

void Vertex::SetPosition(float x, float y, float z)
{
	position[0] = x;
	position[1] = y;
	position[2] = z;
}

void Vertex::SetPosition(float * position)
{
	if (position == nullptr)
		return;

	position = this->position;
}

void Vertex::SetUV(float v1, float v2, float v3)
{
	position[0] = v1;
	position[1] = v2;
	position[2] = v3;
}

void Vertex::SetUV(float * UV)
{
	if (UV == nullptr)
		return;

	UV = this->UV;
}

void Vertex::SetColor(float r, float g, float b, float a)
{
	position[0] = r;
	position[1] = g;
	position[2] = b;
	position[3] = a;
}

void Vertex::SetColor(float * color)
{
	this->color = color;
}

void Vertex::SetNormal(float x, float y, float z)
{
	normal[0] = x;
	normal[1] = y;
	normal[2] = z;
}

void Vertex::SetNormal(float * normal)
{
	if (normal == nullptr)
		return;

	normal = this->normal;
}

void Vertex::SetTangent(float x, float y, float z)
{
	tangent[0] = x;
	tangent[1] = y;
	tangent[2] = z;
}

void Vertex::SetTangent(float * tangent)
{
	if (tangent == nullptr)
		return;

	tangent = this->tangent;
}

void Vertex::SetBitangent(float x, float y, float z)
{
	bitangent[0] = x;
	bitangent[1] = y;
	bitangent[2] = z;
}

void Vertex::SetBitangent(float * bitanegnt)
{
	if (bitangent == nullptr)
		return;

	bitangent = this->bitangent;
}

float * Vertex::GetPositionPtr() const
{
	return position;
}

float * Vertex::GetUVPtr() const
{
	return UV;
}

float * Vertex::GetColorPtr() const
{
	return color;
}

float * Vertex::GetNormalPtr() const
{
	return normal;
}

float * Vertex::GetTangentPtr() const
{
	return normal;
}

float * Vertex::GetBitangentPtr() const
{
	return bitangent;
}
