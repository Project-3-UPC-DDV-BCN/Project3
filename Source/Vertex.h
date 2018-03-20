#pragma once


class Vertex
{
public:
	Vertex();
	~Vertex();

	void SetPosition(float x, float y, float z);
	void SetPosition(float* position);

	void SetUV(float v1, float v2, float v3);
	void SetUV(float* UV);

	void SetColor(float r, float g, float b, float a);
	void SetColor(float* color);

	void SetNormal(float x, float y, float z);
	void SetNormal(float* normal);

	void SetTangent(float x, float y, float z);
	void SetTangent(float* tangent);

	void SetBitangent(float x, float y, float z);
	void SetBitangent(float* bitanegnt);

	float* GetPositionPtr() const;
	float* GetUVPtr() const;
	float* GetColorPtr() const;
	float* GetNormalPtr() const;
	float* GetTangentPtr() const;
	float* GetBitangentPtr() const;


private:
	float* position = nullptr;
	float* UV = nullptr;
	float* color = nullptr;
	float* normal = nullptr;
	float* tangent = nullptr;
	float* bitangent = nullptr;
};