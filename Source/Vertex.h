#pragma once


class Vertex
{
	Vertex();
	virtual ~Vertex();

	void SetPosition(float x, float y, float z);
	void SetPosition(float* position);

	void SetUV(float v1, float v2, float v3);
	void SetUV(float* UV);

	void SetColor(float x, float y, float z);
	void SetColor(float* position);

	void SetNormal(float v1, float v2, float v3);
	void SetNormal(float* UV);

	void SetTangent(float v1, float v2, float v3);
	void SetTangent(float* UV);

	void SetBitangent(float v1, float v2, float v3);
	void SetBitangent(float* UV);


private:
	float* position = nullptr;
	float* UV = nullptr;
	float* color = nullptr;
	float* normal = nullptr;
	float* tangent = nullptr;
	float* bitangent = nullptr;
};